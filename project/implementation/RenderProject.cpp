#include "RenderProject.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "Skybox.h"
#include "Player.h"
#include "PlayerCamera.h"
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
	ShaderPtr basicShader = bRenderer().getObjects()->loadShaderFile("basic", 1, false, true, true, true, false);
	ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 1, false, true, true, true, false);
    ShaderPtr skyboxShader = bRenderer().getObjects()->loadShaderFile("skybox", 1, false, true, true, true, false);
    ShaderPtr playerShader = bRenderer().getObjects()->loadShaderFile("player", 1, false, true, true, true, false);

	// PROPERTIES FOR THE MODELS
    PropertiesPtr treeProperties = bRenderer().getObjects()->createProperties("treeProperties");
    PropertiesPtr sunProperties = bRenderer().getObjects()->createProperties("sunProperties");
    PropertiesPtr skyboxProperties = bRenderer().getObjects()->createProperties("skyboxProperties");
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");

	// BLENDER MODELS (.obj)

    bRenderer().getObjects()->loadObjModel("tree.obj", false, true, basicShader, treeProperties);
    bRenderer().getObjects()->loadObjModel("sun.obj", false, true, basicShader, sunProperties);
    bRenderer().getObjects()->loadObjModel("Crystal.obj", false, true, basicShader, nullptr);


    // SKYBOX (with CubeMap)
    //    TextureData left = TextureData("left.png");
    //    TextureData right = TextureData("right.png");
    //    TextureData bottom = TextureData("bottom.png");
    //    TextureData top = TextureData("top.png");
    //    TextureData front = TextureData("front.png");
    //    TextureData back = TextureData("back.png");
    //    CubeMapPtr skyBoxCubeMapPtr = bRenderer().getObjects()->createCubeMap("skyBoxCubeMap", std::vector<TextureData>{left, right, bottom, top, front, back});
    MaterialPtr skyboxMaterial = bRenderer().getObjects()->loadObjMaterial("skybox.mtl", "skybox", skyboxShader);
    Skybox skybox = Skybox(skyboxMaterial, skyboxProperties);
    ModelPtr skyBoxModel = skybox.generate();
    bRenderer().getObjects()->addModel("skybox", skyBoxModel);
    //    bRenderer().getObjects()->addCubeMap("skyBoxCubeMap", skyBoxCubeMapPtr);

    // PLAYER //
    _player = PlayerPtr(new Player("guy.obj", "guy", "guyProperties", playerShader, getProjectRenderer(), vmml::Vector3f(10.0, 0.0, 10.0), 0.0, -90.0, 0.0, 2.0));

    // TERRAIN LOADER //
    _terrainLoader = TerrainLoaderPtr(new TerrainLoader(getProjectRenderer(), terrainShader, _player));
    
    // SHADOWMODELRENDERER
    _shadowModelRenderer = ShadowModelRendererPtr(new ShadowModelRenderer(getProjectRenderer(), _player, _terrainLoader));
    
    // BLOOMRENDERER
    _bloomRenderer = BloomRendererPtr(new BloomRenderer(getProjectRenderer(), _terrainLoader));

	// create camera
    bRenderer().getObjects()->createCamera("camera");
    
    // create player camera
    _playerCamera = PlayerCameraPtr(new PlayerCamera("camera", _player, getProjectRenderer()));

	// create lights
     bRenderer().getObjects()->createLight("sun", vmml::Vector3f(500, 1000, 500), vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 1.0f, 1.0f, 100.f);
    
	// Update render queue
    updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
	// bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    // std::cout << "FPS: " << std::to_string(1 / deltaTime) << std::endl;

    /* SHADOW MAPPING */
    _shadowModelRenderer->doShadowRenderPass("terrain", deltaTime, elapsedTime);

    /* RENDER PLAYER */
    _player->process("camera", deltaTime);
    
    /* MOVE PLAYER CAMERA (relative to player-position) */
    _playerCamera->move();
    
    /* Add Models to the RenderQueue */
    updateRenderQueue("camera", deltaTime);
    
    /* BLOOM POSTPROCESSING */
    _bloomRenderer->doBloomRenderPass("camera", deltaTime);
    
    bRenderer().getModelRenderer()->clearQueue();
    
	// Quit renderer when escape is pressed
	if (bRenderer().getInput()->getKeyState(bRenderer::KEY_ESCAPE) == bRenderer::INPUT_PRESS)
		bRenderer().terminateRenderer();
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

    ///// Skybox ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(_player->getPosition().x(), 0.0, _player->getPosition().z())) *
        vmml::create_scaling(vmml::Vector3f(1.0));
    // set CubeMap for skybox texturing
    skybox = bRenderer().getObjects()->getShader("skybox");
    skybox->setUniform("CubeMap", bRenderer().getObjects()->getCubeMap("skyBoxCubeMap"));
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model instance
    bRenderer().getModelRenderer()->queueModelInstance("skybox", "skybox", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
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

