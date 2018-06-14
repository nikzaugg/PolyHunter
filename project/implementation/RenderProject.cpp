#include "RenderProject.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "Skydome.h"
#include "Sun.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Cam.h"
#include "ShadowModelRenderer.h"
#include "StartScreenRenderer.h"
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
	ShaderPtr basicShader = bRenderer().getObjects()->loadShaderFile("basic", 2, false, true, true, true, false);
	ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 2, false, true, true, true, false);
    ShaderPtr skydomeShader = bRenderer().getObjects()->loadShaderFile("skydome", 1, false, true, true, true, false);
    ShaderPtr torchLightShader = bRenderer().getObjects()->loadShaderFile("torchLight", 3, false, true, true, true, false);
	ShaderPtr sunShader = bRenderer().getObjects()->loadShaderFile_o("sun", 0, AMBIENT_LIGHTING);
	
	// PROPERTIES FOR THE MODELS
    PropertiesPtr treeProperties = bRenderer().getObjects()->createProperties("treeProperties");
    PropertiesPtr skydomeProperties = bRenderer().getObjects()->createProperties("skydomeProperties");

	// BLENDER MODELS (.obj)
    bRenderer().getObjects()->loadObjModel("tree.obj", false, true, basicShader, treeProperties);
    bRenderer().getObjects()->loadObjModel("crystal.obj", false, true, basicShader, nullptr);
    bRenderer().getObjects()->loadObjModel("sun.obj", false, true, sunShader, nullptr);
    bRenderer().getObjects()->loadObjModel("torch.obj", false, true, torchLightShader, nullptr);
    bRenderer().getObjects()->loadObjModel("skydome.obj", false, true, skydomeShader, nullptr);
    
    // SKYDOME
    _skydome = SkydomePtr(new Skydome("skydome", getProjectRenderer()));
    
    // FOG
    _fogColor = _skydome->getSkyColor() * 0.8;
    _fogDensity = 0.005;
    _fogGradient = 10.00;
    
    // Send fog-variables to shader
    updateFogVariables("basic");
    updateFogVariables("terrain");
    basicShader->setUniform("skyColor", _skydome->getSkyColor());
    terrainShader->setUniform("skyColor", _skydome->getSkyColor());
	
    // GUI CRYSTAL ICON
    bRenderer().getObjects()->createSprite("crystal_icon", "crystal_icon.png");
    
    // GUI TEXT
    FontPtr font = bRenderer().getObjects()->loadFont("KozGoPro-ExtraLight.otf", 50);
    bRenderer().getObjects()->createTextSprite("gui-crystal-info", vmml::Vector3f(1.f, 1.f, 1.f), " ", font);
    
    // SUN
    _sun = SunPtr(new Sun("sun.obj", "sun", "sunProperties", sunShader, getProjectRenderer(), vmml::Vector3f(0.0f, 100.0f, 0.0f), 0.0f, 0.0f, 0.0f, 3.0f));
    
    // PLAYER - FPS-CAMERA
    bRenderer().getObjects()->createCamera("camera");

    _cam = CamPtr(new Cam(getProjectRenderer()));


    
    // TORCH-LIGHTS
    bRenderer().getObjects()->createLight("torch", -bRenderer().getObjects()->getCamera("camera")->getPosition(), vmml::Vector3f(0.92, 1.0, 0.99), vmml::Vector3f(1.0), 1400.0, 0.9, 100.0);
    bRenderer().getObjects()->getShader("basic")->setUniform("torchDamper", _torchDamper);
    bRenderer().getObjects()->getShader("basic")->setUniform("torchInnerCutOff", _torchInnerCutOff);
    bRenderer().getObjects()->getShader("basic")->setUniform("torchOuterCutOff", _torchOuterCutOff);
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchDamper", _torchDamper);
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchInnerCutOff", _torchInnerCutOff);
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchOuterCutOff", _torchOuterCutOff);
    
    // TERRAIN LOADER //
    _terrainLoader = TerrainLoaderPtr(new TerrainLoader(getProjectRenderer(), terrainShader, _cam));
    
    // SHADOWMODELRENDERER
    _shadowModelRenderer = ShadowModelRendererPtr(new ShadowModelRenderer(getProjectRenderer(), _cam, _terrainLoader));
    
    // BLOOMRENDERER
    _bloomRenderer = BloomRendererPtr(new BloomRenderer(getProjectRenderer(), _terrainLoader));
    
	// START SCREEN
	_startScreenRenderer = StartScreenRendererPtr(new StartScreenRenderer(getProjectRenderer(), _cam, _terrainLoader, _viewMatrixHUD));

    // SSAO FBO
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO");
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO2");
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO3");
    bRenderer().getObjects()->createFramebuffer("SSAO_FBO4");
    bRenderer().getObjects()->createDepthMap("ssao_scene_depth", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_normal_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_noise_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createTexture("ssao_blurred_texture", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createSprite("randomNoiseSprite", "bigNoise.png");
    
    // Shader to create occlusion-Texture
    ShaderPtr ssaoShader = bRenderer().getObjects()->loadShaderFile("ssaoShader", 0);
    MaterialPtr ssaoMaterial = bRenderer().getObjects()->createMaterial("ssaoMaterial", ssaoShader);
    bRenderer().getObjects()->createSprite("ssaoSprite", ssaoMaterial);
    
    // Shader to create blurred occlusion-Texture
    ShaderPtr ssaoBlurShader = bRenderer().getObjects()->loadShaderFile("ssaoBlurShader", 0);
    MaterialPtr ssaoBlurMaterial = bRenderer().getObjects()->createMaterial("ssaoBlurMaterial", ssaoBlurShader);
    bRenderer().getObjects()->createSprite("ssaoBlurSprite", ssaoBlurMaterial);
    
	// Update render queue
    updateRenderQueue("camera", 0.0f);

	currentSecond = 0;
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
	// bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
	// std::cout << "FPS: " << std::to_string(1 / deltaTime) << std::endl;
    bool SSAO = true;
    vmml::Matrix4f modelMatrix;
    _startScreenRenderer->bindBlurFbo();
//
//    /* SHADOW MAPPING */
//    _shadowModelRenderer->doShadowRenderPass("terrain", deltaTime, elapsedTime);
//
//    // check for collisions of the player with crystals
//    checkCollision();
//
//    /* Add Models to the RenderQueue */
//    updateRenderQueue("camera", deltaTime);
//
//    /* BLOOM POSTPROCESSING */
//    /* Terrain is loaded inside _bloomRenderer */
//    /* Render Queue is drawn inside _bloomRenderer */
//    //_bloomRenderer->doBloomRenderPass("camera", deltaTime);
//
//    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
//    bRenderer().getModelRenderer()->clearQueue();
    
    if (SSAO) {
        GLint defaultFBO = Framebuffer::getCurrentFramebuffer();
        _cam->process("camera", deltaTime);
        
        /**************************
         * BIND FBO 1
         *************************/
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->bind(false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->bindDepthMap(bRenderer().getObjects()->getDepthMap("ssao_scene_depth"), false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->bindTexture(bRenderer().getObjects()->getTexture("ssao_normal_texture"), false);
        
        // render depth and normals to texture
        bRenderer().getObjects()->getShader("basic")->setUniform("writeNormalsOnly", static_cast<GLfloat>(true));
        bRenderer().getObjects()->getShader("terrain")->setUniform("writeNormalsOnly", static_cast<GLfloat>(true));
        _terrainLoader->drawNormalsOnly("camera", deltaTime, "tree");
        _terrainLoader->drawNormalsOnly("camera", deltaTime, "crystal");
        _terrainLoader->drawNormalsOnly("camera", deltaTime, "terrain");
        
        bRenderer().getObjects()->getShader("basic")->setUniform("writeNormalsOnly", static_cast<GLfloat>(false));
        bRenderer().getObjects()->getShader("terrain")->setUniform("writeNormalsOnly", static_cast<GLfloat>(false));
        
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO")->unbind();
        
        /**************************
         * BIND FBO 3
         *************************/
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO3")->bind(false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO3")->bindTexture(bRenderer().getObjects()->getTexture("ssao_noise_texture"), false);
        
        modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5)) *
        vmml::create_scaling(vmml::Vector3f(1.0));
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("randomNoiseSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO3")->unbind();
        
        /**************************
         * BIND FBO 4
         *************************/
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO4")->bind(false);
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO4")->bindTexture(bRenderer().getObjects()->getTexture("ssao_texture"), false);
        
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<vmml::Vector3f> ssaoKernel;
        float kernelSize = 16.0;
        for (unsigned int i = 0; i<kernelSize; ++i) {
            vmml::Vector3f sample(randomFloats(generator) * 2.0 - 1.0,
                                  randomFloats(generator) * 2.0 - 1.0,
                                  randomFloats(generator)
                                  );
            sample = vmml::normalize(sample);
            sample *= randomFloats(generator);
            float scale = (float)i/kernelSize;
            scale = lerp(0.1, 1.0, scale*scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }
        
        for (unsigned int i = 0; i<kernelSize; ++i) {
            bRenderer().getObjects()->getShader("ssaoShader")->setUniform("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        }
        
        
        modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5)) *
        vmml::create_scaling(vmml::Vector3f(1.0));
        bRenderer().getObjects()->getShader("ssaoShader")->setUniform("ViewMatrix", bRenderer().getObjects()->getCamera("camera")->getViewMatrix());
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("depthMap", bRenderer().getObjects()->getDepthMap("ssao_scene_depth"));
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("normalMap", bRenderer().getObjects()->getTexture("ssao_normal_texture"));
        bRenderer().getObjects()->getMaterial("ssaoMaterial")->setTexture("noiseTex", bRenderer().getObjects()->getTexture("ssao_noise_texture"));
        
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("ssaoSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        
        // RENDER TO DEFAULT FRAMEBUFFER
        bRenderer().getObjects()->getFramebuffer("SSAO_FBO4")->unbind(defaultFBO); //unbind (original fbo will be bound)
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
        
        modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5)) * vmml::create_scaling(vmml::Vector3f(1.0));
        
        bRenderer().getObjects()->getMaterial("ssaoBlurMaterial")->setTexture("ssao_Texture", bRenderer().getObjects()->getTexture("ssao_texture"));
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("ssaoBlurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
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

    _startScreenRenderer->showStartScreen();
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
    // update skyColor
    vmml::Vector3f skyColor = _skydome->getSkyColor();
    vmml::Vector3f newSkyColor = skyColor * 1.1;
    _skydome->setSkyColor(newSkyColor);
    
    // updateFogVariables
    _fogDensity = _fogDensity * 0.97;
    _fogGradient = _fogGradient * 1.05;
    updateFogVariables("basic");
    updateFogVariables("terrain");
    
    // update sun values
    float currentSunIntensity = _sun->getSunIntensity();
    _sun->updateSunIntensityInShader("terrain", currentSunIntensity * 1.1);
    _sun->updateSunIntensityInShader("basic", currentSunIntensity * 1.1);
    _sun->updateSunIntensityInShader("torchLight", currentSunIntensity * 1.1);
    _sun->increaseSunSize(0.1);
    _sun->increaseHealth(0.1);
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
    _skydome->setSkyColor(skyColor);
    _skydome->setSkydomeGradient(gradient);
    _skydome->setSkydomeDensity(density);
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
    
	//if ((int)elapsedTime % 10 == 0 && currentSecond != (int)elapsedTime) {
	//	std::cout << elapsedTime << std::endl;
	//	currentSecond = (int)elapsedTime;
	//	_sun->setIntensity(0.1 * currentSecond);
	//}

    bRenderer().getObjects()->getShader("basic")->setUniform("playerPos", _cam->getPosition());
    bRenderer().getObjects()->getShader("terrain")->setUniform("playerPos", _cam->getPosition());
    
    /// Skydome ///
    _skydome->render(camera, _cam->getPosition());
    
    ///*** Torch Light ***/
    // Position the torch relative to the camera
    bRenderer().getObjects()->getShader("basic")->setUniform("torchDir", bRenderer().getObjects()->getCamera("camera")->getForward());
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchDir", bRenderer().getObjects()->getCamera("camera")->getForward());
    // place torch-light
    bRenderer().getObjects()->getLight("torch")->setPosition(_cam->getPosition() - bRenderer().getObjects()->getCamera("camera")->getForward()*10.0f);
    
    ///*** Torch - little crystals ***/
    float origX = 0.75;
    float origZ = 0.8;
    int gridX = _terrainLoader->getPlayerGridX();
    int gridZ = _terrainLoader->getPlayerGridZ();
    std::string currentTerrainKey = _terrainLoader->generateTerrainKey(gridX, gridZ);
    TerrainPtr currentTerrain = _terrainLoader->getSingleTerrain(currentTerrainKey);
    int nrOfCrystals = currentTerrain->getNrOfCrystalsCollected();
    
    for (int i = 1; i <= nrOfCrystals; i++) {
        float angle = 2*M_PI_F / nrOfCrystals;
        angle *= i + ((M_PI_F/20.0)*elapsedTime);
    
        float x = origX + 0.3 * cos(angle);
        float z = origZ + 0.3 * sin(angle);
        
        modelMatrix = bRenderer().getObjects()->getCamera(camera)->getInverseViewMatrix();        // position and orient to match camera
        modelMatrix *= vmml::create_translation(vmml::Vector3f(x, -0.7, z)) * vmml::create_scaling(vmml::Vector3f(0.015f)) *
        vmml::create_rotation(-0.82f, vmml::Vector3f::UNIT_Z);
        modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f(0.5, 0.5, -0.5));
        bRenderer().getModelRenderer()->queueModelInstance("torch", &"torch_particle_"[i], camera, modelMatrix, std::vector<std::string>({ "sun", "torch" }));
    }

	/// SUN (Torch) ///
	_sun->render(camera, _cam->getPosition(), _viewMatrixHUD, elapsedTime);
}

/* Camera movement */
void RenderProject::updateCamera(const std::string &camera, const double &deltaTime)
{
	//// Adjust aspect ratio ////
	bRenderer().getObjects()->getCamera(camera)->setAspectRatio(bRenderer().getView()->getAspectRatio());
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

