#include "RenderProject.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "Skybox.h"
#include "Sun.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Cam.h"
#include "ShadowModelRenderer.h"

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
    ShaderPtr skyboxShader = bRenderer().getObjects()->loadShaderFile("skybox", 1, false, true, true, true, false);
    ShaderPtr torchLightShader = bRenderer().getObjects()->loadShaderFile("torchLight", 4, false, true, true, true, false);
	ShaderPtr sunShader = bRenderer().getObjects()->loadShaderFile_o("sun", 0, AMBIENT_LIGHTING);
	
	// PROPERTIES FOR THE MODELS
    PropertiesPtr treeProperties = bRenderer().getObjects()->createProperties("treeProperties");
    PropertiesPtr skyboxProperties = bRenderer().getObjects()->createProperties("skyboxProperties");
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");

	// BLENDER MODELS (.obj)
    bRenderer().getObjects()->loadObjModel("torch.obj", false, true, torchLightShader, nullptr);

    // SKYBOX
    MaterialPtr skyboxMaterial = bRenderer().getObjects()->loadObjMaterial("skybox.mtl", "skybox", skyboxShader);
    _skybox = SkyboxPtr(new Skybox(skyboxMaterial, skyboxProperties, getProjectRenderer()));
	_skybox->setSkyColor(vmml::Vector3f(0.026, 0.048, 0.096));
    //skybox->setSkyColor(vmml::Vector3f(0.25));

    // FOG
    _fogColor = vmml::Vector3f(0.05);
    _fogDensity = 0.005;
    _fogGradient = 10.00;
    
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
    _cam = CamPtr(new Cam(getProjectRenderer()));
    
    // TORCH-LIGHT
    bRenderer().getObjects()->createLight("torch", -bRenderer().getObjects()->getCamera("camera")->getPosition(), vmml::Vector3f(0.92, 1.0, 0.99), vmml::Vector3f(1.0), 1400.0, 0.9, 100.0);

    // TERRAIN LOADER //
    _terrainLoader = TerrainLoaderPtr(new TerrainLoader(getProjectRenderer(), terrainShader, _cam));
    
    // SHADOWMODELRENDERER
    _shadowModelRenderer = ShadowModelRendererPtr(new ShadowModelRenderer(getProjectRenderer(), _cam, _terrainLoader));
    
    // BLOOMRENDERER
    _bloomRenderer = BloomRendererPtr(new BloomRenderer(getProjectRenderer(), _terrainLoader));
    
	// Update render queue
    updateRenderQueue("camera", 0.0f);

	currentSecond = 0;
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
	// bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    // std::cout << "FPS: " << std::to_string(1 / deltaTime) << std::endl;

    /* SHADOW MAPPING */
    _shadowModelRenderer->doShadowRenderPass("terrain", deltaTime, elapsedTime);
    
    // check for collisions of the player with crystals
    checkCollision();

    /* Add Models to the RenderQueue */
    updateRenderQueue("camera", deltaTime);

    /* BLOOM POSTPROCESSING */
    /* Terrain is loaded inside _bloomRenderer */
    /* Render Queue is drawn inside _bloomRenderer */
    _bloomRenderer->doBloomRenderPass("camera", deltaTime);
    
    /*** GUI - Crystal Icon ***/
    // translate and scale
    GLfloat titleScale = 0.1f;
    vmml::Matrix4f scaling = vmml::create_scaling(vmml::Vector3f(titleScale / bRenderer().getView()->getAspectRatio(), titleScale, titleScale));
    vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(-0.95f, 0.9f, -0.65f)) * scaling;
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
    
	//if ((int)elapsedTime % 10 == 0 && currentSecond != (int)elapsedTime) {
	//	std::cout << elapsedTime << std::endl;
	//	currentSecond = (int)elapsedTime;
	//	_sun->setIntensity(0.1 * currentSecond);
	//}

    bRenderer().getObjects()->getShader("basic")->setUniform("playerPos", _cam->getPosition());
    bRenderer().getObjects()->getShader("terrain")->setUniform("playerPos", _cam->getPosition());
    
    /////// Skybox ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(_cam->getPosition().x(), 0.0, _cam->getPosition().z())) *
        vmml::create_scaling(vmml::Vector3f(1.0));
    // set CubeMap for skybox texturing
    skybox = bRenderer().getObjects()->getShader("skybox");
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    bRenderer().getModelRenderer()->queueModelInstance("skybox", "skybox_instance", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);

    ///*** Torch ***/
    // Position the torch relative to the camera
    bRenderer().getObjects()->getShader("basic")->setUniform("torchDir", bRenderer().getObjects()->getCamera("camera")->getForward());
    bRenderer().getObjects()->getShader("basic")->setUniform("torchInnerCutOff", cos(M_PI_F/180 * 15.0));
    bRenderer().getObjects()->getShader("basic")->setUniform("torchOuterCutOff", cos(M_PI_F/180 * 20.0));
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchDir", bRenderer().getObjects()->getCamera("camera")->getForward());
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchInnerCutOff", cos(M_PI_F/180 * 15.0));
    bRenderer().getObjects()->getShader("terrain")->setUniform("torchOuterCutOff", cos(M_PI_F/180 * 20.0));
    bRenderer().getObjects()->getLight("torch")->setPosition(_cam->getPosition() - bRenderer().getObjects()->getCamera("camera")->getForward()*10.0f);
    modelMatrix = bRenderer().getObjects()->getCamera(camera)->getInverseViewMatrix();        // position and orient to match camera
    modelMatrix *= vmml::create_translation(vmml::Vector3f(0.75f, -0.70f, 0.8f)) * vmml::create_scaling(vmml::Vector3f(0.15f)) * vmml::create_rotation(1.64f, vmml::Vector3f::UNIT_Y);
    modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f::UNIT_Y);
    // submit to render queue
    bRenderer().getModelRenderer()->queueModelInstance("torch", "torch_instance", camera, modelMatrix, std::vector<std::string>({ "sun" }));
    
    ///*** Torch - Particles ***/
    float origX = 0.8;
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
        
        std::cout << "x " << x << std::endl;
        std::cout << "y " << z << std::endl;
        modelMatrix = bRenderer().getObjects()->getCamera(camera)->getInverseViewMatrix();        // position and orient to match camera
        modelMatrix *= vmml::create_translation(vmml::Vector3f(x, -0.2f, z)) * vmml::create_scaling(vmml::Vector3f(0.02f)) *
        vmml::create_rotation(-0.82f, vmml::Vector3f::UNIT_Z);
        modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f::UNIT_Y);
        bRenderer().getModelRenderer()->queueModelInstance("torch", &"torch_particle_"[i], camera, modelMatrix, std::vector<std::string>({ "torch" }));
    }

    
//    ///*** Torch - Particles ***/
//    modelMatrix = bRenderer().getObjects()->getCamera(camera)->getInverseViewMatrix();        // position and orient to match camera
//    modelMatrix *= vmml::create_translation(vmml::Vector3f(0.75f, -0.2f, 0.3)) * vmml::create_scaling(vmml::Vector3f(0.02f)) *
//    vmml::create_rotation(-0.82f, vmml::Vector3f::UNIT_Z);
//    modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f::UNIT_Y);
//    bRenderer().getModelRenderer()->queueModelInstance("torch", "torch_particle_2", camera, modelMatrix, std::vector<std::string>({ "sun" }));
//
//    ///*** Torch - Particles ***/
//    modelMatrix = bRenderer().getObjects()->getCamera(camera)->getInverseViewMatrix();        // position and orient to match camera
//    modelMatrix *= vmml::create_translation(vmml::Vector3f(0.5f, -0.2f, 0.3)) * vmml::create_scaling(vmml::Vector3f(0.02f)) *
//    vmml::create_rotation(-0.82f, vmml::Vector3f::UNIT_Z);
//    modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f::UNIT_Y);
//    bRenderer().getModelRenderer()->queueModelInstance("torch", "torch_particle_3", camera, modelMatrix, std::vector<std::string>({ "sun" }));
//
//    ///*** Torch - Particles ***/
//    modelMatrix = bRenderer().getObjects()->getCamera(camera)->getInverseViewMatrix();        // position and orient to match camera
//    modelMatrix *= vmml::create_translation(vmml::Vector3f(0.75f, -0.2f, 0.8)) * vmml::create_scaling(vmml::Vector3f(0.02f)) *
//    vmml::create_rotation(-0.82f, vmml::Vector3f::UNIT_Z);
//    modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f::UNIT_Y);
//    bRenderer().getModelRenderer()->queueModelInstance("torch", "torch_particle_4", camera, modelMatrix, std::vector<std::string>({ "sun" }));
    
    
	/// SUN ///
	_sun->render(camera, _cam->getPosition(), _viewMatrixHUD);
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

