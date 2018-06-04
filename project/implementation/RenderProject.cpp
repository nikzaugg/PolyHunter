#include "RenderProject.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "Skybox.h"
#include "Sun.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Cam.h"
#include "ShadowModelRenderer.h"
#include <random>

/* Initialize the Project */
void RenderProject::init()
{
	bRenderer::loadConfigFile("config.json");	// load custom configurations replacing the default values in Configuration.cpp

	// let the renderer create an OpenGL context and the main window
	if(Input::isTouchDevice())
		bRenderer().initRenderer(true);										// full screen on iOS
	else
		bRenderer().initRenderer(1920, 1080, false, "PolyHunter");		// windowed mode on desktop
		//bRenderer().initRenderer(View::getScreenWidth(), View::getScreenHeight(), true);		// full screen using full width and height of the screen

	// start main loop
	bRenderer().runRenderer();
}

/* This function is executed when initializing the renderer */
void RenderProject::initFunction()
{
	// get OpenGL and shading language version
	bRenderer::log("OpenGL Version: ", glGetString(GL_VERSION));
	bRenderer::log("Shading Language Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// initialize variables
	_offset = 0.0f;
	_randomOffset = 0.0f;
	_cameraSpeed = 40.0f;
	_running = false; _lastStateSpaceKey = bRenderer::INPUT_UNDEFINED;
	_viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    _animation_forward = true;
    _animation = 0.0;
    _animationSpeed = 20.0;

	// set shader versions (optional)
	bRenderer().getObjects()->setShaderVersionDesktop("#version 120");
	bRenderer().getObjects()->setShaderVersionES("#version 100");

	// SHADERS
	ShaderPtr basicShader = bRenderer().getObjects()->loadShaderFile("basic", 1, false, true, true, true, false);
	ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 1, false, true, true, true, false);
    ShaderPtr skyboxShader = bRenderer().getObjects()->loadShaderFile("skybox", 1, false, true, true, true, false);
    ShaderPtr playerShader = bRenderer().getObjects()->loadShaderFile("player", 1, false, true, true, true, false);
	ShaderPtr sunShader = bRenderer().getObjects()->loadShaderFile_o("sunShader", 0, AMBIENT_LIGHTING);
    // SSAO-Shaders
    ShaderPtr positionAndDepthToTextureShader = bRenderer().getObjects()->loadShaderFile("ssao_pos_depthShader", 1, false, true, true, true, false);
    ShaderPtr normalToTextureShader = bRenderer().getObjects()->loadShaderFile("ssao_normalShader", 1, false, true, true, true, false);
//    ShaderPtr blurToTextureShader = bRenderer().getObjects()->loadShaderFile("ssao_blurShader", 1, false, true, true, true, false);
	
	// PROPERTIES FOR THE MODELS
    PropertiesPtr treeProperties = bRenderer().getObjects()->createProperties("treeProperties");
    PropertiesPtr skyboxProperties = bRenderer().getObjects()->createProperties("skyboxProperties");
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");

	// BLENDER MODELS (.obj)
    // bRenderer().getObjects()->loadObjModel("tree.obj", false, true, basicShader, treeProperties);

    // SKYBOX
    MaterialPtr skyboxMaterial = bRenderer().getObjects()->loadObjMaterial("skybox.mtl", "skybox", skyboxShader);
    _skybox = SkyboxPtr(new Skybox(skyboxMaterial, skyboxProperties, getProjectRenderer()));
	_skybox->setSkyColor(vmml::Vector3f(0.26, 0.48, 0.96));
    //skybox->setSkyColor(vmml::Vector3f(0.25));

    // FOG
    _fogColor = vmml::Vector3f(0.5);
    _fogDensity = 0.005;
    _fogGradient = 5.00;
    
    // Send fog-variables to shader
    updateFogVariables("basic");
    updateFogVariables("terrain");
    basicShader->setUniform("skyColor", _skybox->getSkyColor());
    terrainShader->setUniform("skyColor", _skybox->getSkyColor());
	
    // GUI CRYSTAL ICON
    bRenderer().getObjects()->createSprite("crystal_icon", "crystal_icon.png");
    
    // GUI TEXT
    FontPtr font = bRenderer().getObjects()->loadFont("KozGoPro-ExtraLight.otf", 50);
    bRenderer().getObjects()->createTextSprite("gui-crystal-info", vmml::Vector3f(1.f, 1.f, 1.f), " ", font);
    
    // SUN
    _sun = SunPtr(new Sun("sun.obj", "sun", "sunProperties", sunShader, getProjectRenderer(), vmml::Vector3f(0.0f, 100.0f, 0.0f), 0.0f, 0.0f, 0.0f, 3.0f));

    // PLAYER - FPS-CAMERA
    bRenderer().getObjects()->createCamera("camera");
    bRenderer().getObjects()->getCamera("camera")->setFarClippingPlane(10.0);
    _cam = CamPtr(new Cam(getProjectRenderer()));

    // TERRAIN LOADER //
    _terrainLoader = TerrainLoaderPtr(new TerrainLoader(getProjectRenderer(), terrainShader, _cam));
    
    // SHADOWMODELRENDERER
    _shadowModelRenderer = ShadowModelRendererPtr(new ShadowModelRenderer(getProjectRenderer(), _cam, _terrainLoader));
    
    // BLOOMRENDERER
    _bloomRenderer = BloomRendererPtr(new BloomRenderer(getProjectRenderer(), _terrainLoader));
    
    // SSAO FBO
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO");
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO2");
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO3");
    bRenderer().getObjects()->createDepthMap("ssao_scene_depth", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_position_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_normal_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_noise_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createSprite("randomNoiseSprite", "randTex.png");
    
    ShaderPtr ssaoShader = bRenderer().getObjects()->loadShaderFile("ssaoShader", 0);
    MaterialPtr ssaoMaterial = bRenderer().getObjects()->createMaterial("ssaoMaterial", ssaoShader);
    bRenderer().getObjects()->createSprite("ssaoSprite", ssaoMaterial);
	
    // Update render queue
    updateRenderQueue("camera", 0.0f);

	currentSecond = 0;
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
    bool SSAO = true;
    vmml::Matrix4f modelMatrix;
    
	// bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    // std::cout << "FPS: " << std::to_string(1 / deltaTime) << std::endl;

    /* SHADOW MAPPING */
    // _shadowModelRenderer->doShadowRenderPass("terrain", deltaTime, elapsedTime);
    
    // check for collisions of the player with crystals
    checkCollision();
    
    /* Add Models to the RenderQueue */
    updateRenderQueue("camera", deltaTime);
    
    /* BLOOM POSTPROCESSING */
    /* Terrain is loaded inside _bloomRenderer */
    /* Render Queue is drawn inside _bloomRenderer */
    // _bloomRenderer->doBloomRenderPass("camera", deltaTime);
    
    if (SSAO) {
        /**************************
         * BIND DEPTH-FBO
         *************************/
        //_renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
        GLint defaultFBO = Framebuffer::getCurrentFramebuffer();
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->bind(false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->bindDepthMap(bRenderer().getObjects()->getDepthMap("ssao_scene_depth"), false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->bindTexture(bRenderer().getObjects()->getTexture("ssao_position_texture"), false);
        
        /********************************************
         * RENDER SCENE TO DEPTH AND POSITION TEXTURE
         *******************************************/
        
        _terrainLoader->drawPositionsOnly("camera", deltaTime, "terrain");
        _terrainLoader->drawPositionsOnly("camera", deltaTime, "tree");
        
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->unbind();
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO2")->bind(false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO2")->bindTexture(bRenderer().getObjects()->getTexture("ssao_normal_texture"), false);
        
        _terrainLoader->drawNormalsOnly("camera", deltaTime, "terrain");
        _terrainLoader->drawNormalsOnly("camera", deltaTime, "tree");
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO2")->unbind();
        
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO3")->bindTexture(bRenderer().getObjects()->getTexture("ssao_noise_texture"), false);
        
        modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5)) *
        vmml::create_scaling(vmml::Vector3f(1.0));
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("randomNoiseSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        
        /**********************************
         * RENDER TO DEFAULT FRAMEBUFFER  *
         * Switch to default framebuffer
         * Output computed ambient-texture
         *
         * render the whole scene with the ssaoShader bound -> need current position information of the fragments
         * like this here, we only have a 2D texture...and not information about positions etc.
         *
         * TODO: find out how to switch shaders!
         * -> write only Positions in ViewSpace to Shader
         * -> write only Normals in ViewSpae to Shader
         * -> In ssaoShader compute TBN to rotate Kernel along tangent-normal
         ***************/
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO3")->unbind(defaultFBO); //unbind (original fbo will be bound)
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
        
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<vmml::Vector3f> ssaoKernel;
        for (unsigned int i = 0; i<32; ++i) {
            vmml::Vector3f sample(randomFloats(generator) * 2.0 - 1.0,
                                  randomFloats(generator) * 2.0 - 1.0,
                                  randomFloats(generator)
                                  );
            sample = vmml::normalize(sample);
            sample *= randomFloats(generator);
            float scale = (float)i/32.0;
            scale = lerp(0.1, 1.0, scale*scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }
        
        for (unsigned int i = 0; i<32; ++i) {
            bRenderer().getObjects()->getShader("ssaoShader")->setUniform("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        }
        
        std::vector<vmml::Vector3f> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++) {
            vmml::Vector3f noise(
                                 randomFloats(generator) * 2.0 - 1.0,
                                 randomFloats(generator) * 2.0 -1.0,
                                 0.0f);
            ssaoNoise.push_back(noise);
        }
        
//        unsigned int noiseTexture;
//        glGenTextures(1, &noiseTexture);
//        glBindTexture(GL_TEXTURE_2D, noiseTexture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//        GLuint ssaoShaderID = bRenderer().getObjects()->getShader("ssaoShader")->getProgramID();
//        glUseProgram(ssaoShaderID);
//
//        GLuint location = glGetUniformLocation(ssaoShaderID, "texNoise");
//        glUniform1i(location, 3);
//
//        glActiveTexture(GL_TEXTURE3);
//        glBindTexture(GL_TEXTURE_2D,  noiseTexture);
        
        modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5)) *
        vmml::create_scaling(vmml::Vector3f(1.0));
        vmml::Matrix4f inverseProj;
        vmml::Matrix4f proj = bRenderer().getObjects()->getCamera("camera")->getProjectionMatrix();
        vmml::compute_inverse(proj, inverseProj);
        bRenderer().getObjects()->getShader("ssaoShader")->setUniform("newProjectionMatrix", proj);
        bRenderer().getObjects()->getShader("ssaoShader")->setUniform("invProjectionMatrix", inverseProj);
        bRenderer().getObjects()->getShader("ssaoShader")->setUniform("ViewMatrix", bRenderer().getObjects()->getCamera("camera")->getViewMatrix());
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("depthMap", bRenderer().getObjects()->getDepthMap("ssao_scene_depth"));
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("positionMap", bRenderer().getObjects()->getTexture("ssao_position_texture"));
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("normalMap", bRenderer().getObjects()->getTexture("ssao_normal_texture"));
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("noiseTex", bRenderer().getObjects()->getTexture("ssao_noise_texture"));
        
        // send depthMap and samples to all shaders
        // draw scene again to generate occlusion texture
        // use occlusion texture to do lighting pass
        
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("ssaoSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    }
    
    /*** GUI - Crystal Icon ***/
    // translate and scale
    GLfloat titleScale = 0.1f;
    vmml::Matrix4f scaling = vmml::create_scaling(vmml::Vector3f(titleScale / bRenderer().getView()->getAspectRatio(), titleScale, titleScale));
    modelMatrix = vmml::create_translation(vmml::Vector3f(-0.95f, 0.9f, -0.65f)) * scaling;
    // draw
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("crystal_icon"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false, false);

    /*** GUI - Crystal Counter Text ***/
    titleScale = 0.1f;
    scaling = vmml::create_scaling(vmml::Vector3f(titleScale / bRenderer().getView()->getAspectRatio(), titleScale, titleScale));
    modelMatrix = vmml::create_translation(vmml::Vector3f(-1.15f / bRenderer().getView()->getAspectRatio(), 0.87f, -0.65f)) * scaling;
    // draw
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getTextSprite("gui-crystal-info"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
	// Quit renderer when escape is pressed
	if (bRenderer().getInput()->getKeyState(bRenderer::KEY_ESCAPE) == bRenderer::INPUT_PRESS)
		bRenderer().terminateRenderer();
}

// checks collision between player and crystals
void RenderProject::checkCollision()
{
    int gridX = _terrainLoader->getPlayerGridX();
    int gridZ = _terrainLoader->getPlayerGridZ();
    std::string currentTerrainKey = _terrainLoader->generateTerrainKey(gridX, gridZ);
    TerrainPtr currentTerrain = _terrainLoader->getSingleTerrain(currentTerrainKey);
    vmml::Vector3f currentPlayerPos = _cam->getPosition();
    currentTerrain->checkCollisionWithEntities(currentPlayerPos);
    int nrOfCrystalsCollected = currentTerrain->getNrOfCrystalsCollected();
    if (nrOfCrystalsCollected > _nrOfCollectedCrystals) {
        // handle crystal addition
        updateGameVariables();
        _nrOfCollectedCrystals = nrOfCrystalsCollected;
    }
    bRenderer().getObjects()->getTextSprite("gui-crystal-info")->setText(std::to_string(_nrOfCollectedCrystals));
}

// updates gameplay-variables (fog, sun-healt, points etc.)
void RenderProject::updateGameVariables()
{
    float sunHealth = _sun->getHealth();
    vmml::Vector3f skyColor = _skybox->getSkyColor();

    // update sun-health
    sunHealth += 0.05;
    std::cout << sunHealth << std::endl;
    // set new skycolor
    vmml::Vector3f newSkyColor = skyColor * (1.0 + sunHealth);
    _skybox->setSkyColor(newSkyColor);
    
    // set new fogDensity
    
    // set new fogDistance
    
    // updateFogVariables
    updateFogVariables("basic");
    updateFogVariables("terrain");
    
    // updateSkyBoxVariables
}

// updates fog variables in specified shader
void RenderProject::updateFogVariables(std::string shaderName)
{
    ShaderPtr shader = bRenderer().getObjects()->getShader(shaderName);
    shader->setUniform("fogDensity", _fogDensity);
    shader->setUniform("fogGradient", _fogGradient);
    shader->setUniform("fogColor", _fogColor);
}

// updates skybox variables in specified shader
void RenderProject::updateSkyBoxVariables(vmml::Vector3f skyColor, float gradient, float density)
{
    _skybox->setSkyColor(skyColor);
    _skybox->setSkyboxGradient(gradient);
    _skybox->setSkyboxDensity(density);
}

/* function is executed when terminating the renderer */
void RenderProject::terminateFunction()
{
	bRenderer::log("I totally terminated this Renderer :-)");
}

/* Update render queue */
void RenderProject::updateRenderQueue(const std::string &camera, const double &deltaTime)
{
	elapsedTime += deltaTime;
	vmml::Matrix4f modelMatrix;
	ShaderPtr skybox;
    
    _cam->process(camera, deltaTime);
    _terrainLoader->process(camera, deltaTime);
    _sun->render(camera, _cam->getPosition(), _viewMatrixHUD);
    
	//if ((int)elapsedTime % 10 == 0 && currentSecond != (int)elapsedTime) {
	//	std::cout << elapsedTime << std::endl;
	//	currentSecond = (int)elapsedTime;
	//	_sun->setIntensity(0.1 * currentSecond);
	//}

    vmml::Matrix4f cameraView = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
    bRenderer().getObjects()->getShader("basic")->setUniform("playerPos", cameraView * _cam->getPosition());
    bRenderer().getObjects()->getShader("terrain")->setUniform("playerPos", cameraView * _cam->getPosition());
    
    /////// Skybox ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(_cam->getPosition().x(), 0.0, _cam->getPosition().z())) *
        vmml::create_scaling(vmml::Vector3f(1.0));
    // set CubeMap for skybox texturing
    skybox = bRenderer().getObjects()->getShader("skybox");
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw modeld
    bRenderer().getModelRenderer()->queueModelInstance("skybox", "skybox_instance", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
}

/* Camera movement */
void RenderProject::updateCamera(const std::string &camera, const double &deltaTime)
{
	//// Adjust aspect ratio ////
	bRenderer().getObjects()->getCamera(camera)->setAspectRatio(bRenderer().getView()->getAspectRatio());

    // pause using double tap
	if (Input::isTouchDevice()) {
		if (bRenderer().getInput()->doubleTapRecognized()) {
			_running = !_running;
		}
	}
	else {
		GLint currentStateSpaceKey = bRenderer().getInput()->getKeyState(bRenderer::KEY_SPACE);
		if (currentStateSpaceKey != _lastStateSpaceKey)
		{
		    _lastStateSpaceKey = currentStateSpaceKey;
		    if (currentStateSpaceKey == bRenderer::INPUT_PRESS){
		        _running = !_running;
		        bRenderer().getInput()->setCursorEnabled(!_running);
		    }
		}
	}
}

/* For iOS only: Handle device rotation */
void RenderProject::deviceRotated()
{
	if (bRenderer().isInitialized()){
		// set view to full screen after device rotation
		bRenderer().getView()->setFullscreen(true);
		bRenderer::log("Device rotated");
	}
}

/* For iOS only: Handle app going into background */
void RenderProject::appWillResignActive()
{
	if (bRenderer().isInitialized()){
		// stop the renderer when the app isn't active
		bRenderer().stopRenderer();
	}
}

/* For iOS only: Handle app coming back from background */
void RenderProject::appDidBecomeActive()
{
	if (bRenderer().isInitialized()){
		// run the renderer as soon as the app is active
		bRenderer().runRenderer();
	}
}

/* For iOS only: Handle app being terminated */
void RenderProject::appWillTerminate()
{
	if (bRenderer().isInitialized()){
		// terminate renderer before the app is closed
		bRenderer().terminateRenderer();
	}
}

/* Helper functions */
GLfloat RenderProject::randomNumber(GLfloat min, GLfloat max){
	return min + static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / (max - min)));
}

float RenderProject::lerp(float a, float b, float f)
{
    return a + f * (b-a);
}

