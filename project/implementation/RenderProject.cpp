#include "RenderProject.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "Skybox.h"
#include "Sun.h"
#include "Player.h"
#include "PlayerCamera.h"

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
	basicShader->setUniform("skyColor", vmml::Vector3f(0.53f, 0.807f, 0.98f));
	ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 1, false, true, true, true, false);
    ShaderPtr skyboxShader = bRenderer().getObjects()->loadShaderFile("skybox", 1, false, true, true, true, false);
    ShaderPtr playerShader = bRenderer().getObjects()->loadShaderFile("player", 1, false, true, true, true, false);
	ShaderPtr sunShader = bRenderer().getObjects()->loadShaderFile("sun", 1, false, true, true, true, false);

	// PROPERTIES FOR THE MODELS
    PropertiesPtr treeProperties = bRenderer().getObjects()->createProperties("treeProperties");
    PropertiesPtr sunProperties = bRenderer().getObjects()->createProperties("sunProperties");
    PropertiesPtr skyboxProperties = bRenderer().getObjects()->createProperties("skyboxProperties");
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");

	// BLENDER MODELS (.obj)

    bRenderer().getObjects()->loadObjModel("tree.obj", false, true, basicShader, treeProperties);
    bRenderer().getObjects()->loadObjModel("Crystal.obj", false, true, basicShader, nullptr);


    // SKYBOX
    MaterialPtr skyboxMaterial = bRenderer().getObjects()->loadObjMaterial("skybox.mtl", "skybox", skyboxShader);
    Skybox skybox = Skybox(skyboxMaterial, skyboxProperties, getProjectRenderer());
	// Sun
	//Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
	_sun = SunPtr(new Sun("sun_instance", "sun", "sunProperties", sunShader, getProjectRenderer(), vmml::Vector3f(0.0f, 100.0f, 0.0f), 0.0f, 0.0f, 0.0f, 3.0f));
	
    //    bRenderer().getObjects()->addCubeMap("skyBoxCubeMap", skyBoxCubeMapPtr);

    // PLAYER //
    _player = PlayerPtr(new Player("guy.obj", "guy", "guyProperties", playerShader, getProjectRenderer(), vmml::Vector3f(10.0, 0.0, 10.0), 0.0, -90.0, 0.0, 2.0));

    // TERRAIN LOADER //
    _terrainLoader = TerrainLoaderPtr(new TerrainLoader(getProjectRenderer(), terrainShader, _player));

	// create camera
    bRenderer().getObjects()->createCamera("camera");
    _playerCamera = PlayerCameraPtr(new PlayerCamera("camera", _player, getProjectRenderer()));

	// create lights
	bRenderer().getObjects()->createLight("sun", vmml::Vector3f(0.0, 200.0, 0.0), vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 1.0f, 0.5f, 100.0f);
    
    /******************
     BLOOM FBO
     *****************/
    bRenderer().getObjects()->createFramebuffer("bloomFBO");                    // create framebuffer object
    bRenderer().getObjects()->createTexture("bloomTexture", 0.f, 0.f);    // create texture to bind to the fbo
    //bRenderer().getObjects()->createTexture("bloomTexture2", 0.f, 0.f);    // create texture to bind to the fbo
    // load shader that blurs the texture
    ShaderPtr bloomShader = bRenderer().getObjects()->loadShaderFile_o("bloom", 0);
    MaterialPtr bloomMaterial = bRenderer().getObjects()->createMaterial("bloomMaterial", bloomShader);
    // create an empty material to assign either texture1 or texture2 to
    bRenderer().getObjects()->createSprite("bloomSprite", bloomMaterial);
    // create a sprite using the material created above
    
	// Update render queue
	updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
	// bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    // std::cout << "FPS: " << std::to_string(1 / deltaTime) << std::endl;
	//// Draw Scene and do post processing ////
    GLint defaultFBO;

    /*************************
     * BEGIN POSTPROCESSING  *
     ************************/
    //bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());        // reduce viewport size
    //defaultFBO = Framebuffer::getCurrentFramebuffer();    // get current fbo to bind it again after drawing the scene
    //bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("bloomTexture"), false);    // bind the fbo
    
    /***************
     * DRAW SCENE  *
     * everything that gets rendered will be saved
     * inside the texture attachment of the new framebuffer
     **************/
    
    
    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
    bRenderer().getModelRenderer()->clearQueue();
    
    /// Update render queue ///
    updateRenderQueue("camera", deltaTime);
    
    //// Camera Movement ////
    updateCamera("camera", deltaTime);
    
    /// Update camera position according to player's position ///
    _playerCamera->move();
    
    /******************************
     * RENDER TO NEW FRAMEBUFFER  *
     * The scene above is drawn to a sprite using the texture attachment
     *****************************/
    //vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    //bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("bloomTexture"), false);
    //bRenderer().getObjects()->getMaterial("bloomMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("bloomTexture"));
    //bRenderer().getObjects()->getMaterial("bloomMaterial")->setScalar("isVertical", static_cast<GLfloat>(true));
    //// draw currently active framebuffer
    //bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("bloomSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

    /**********************************
     * RENDER TO DEFAULT FRAMEBUFFER  *
     * Switch to detault framebuffer (the screen) and draw the created sprite
     *********************************/
    //bRenderer().getObjects()->getFramebuffer("bloomFBO")->unbind(defaultFBO); //unbind (original fbo will be bound)
    //bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    //// draw draw currently active framebuffer
    //bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("bloomSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    //
	// Quit renderer when escape is pressed
	if (bRenderer().getInput()->getKeyState(bRenderer::KEY_ESCAPE) == bRenderer::INPUT_PRESS)
		bRenderer().terminateRenderer();
}

/* This function is executed when terminating the renderer */
void RenderProject::terminateFunction()
{
	bRenderer::log("I totally terminated this Renderer :-)");
}


/* Update render queue */
void RenderProject::updateRenderQueue(const std::string &camera, const double &deltaTime)
{
	elapsedTime += deltaTime;

	vmml::Matrix4f modelMatrix;
	vmml::Matrix3f normalMatrix;
	ShaderPtr skybox;
    
    /// LOW POLY CRYSTAL ///
    modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 20.0, 0.0)) *
    vmml::create_scaling(vmml::Vector3f(5.0f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("Crystal", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);

    // Move Light to see changes in Colors/Lighting
    // float lightPosition = bRenderer().getObjects()->getLight("sun")->getPosition().z();
    if(_animation_forward)
    {
        if(_animation > 300.0)
        {
            _animation_forward = false;
        } else
        {
            _animation += deltaTime * _animationSpeed;
        }
    } else {
        if(_animation < 0.0)
        {
            _animation_forward = true;
        }
        else
        {
            _animation -= deltaTime * _animationSpeed;
        }
    }

    bRenderer().getObjects()->getLight("sun")->setPosition(vmml::Vector3f(_animation, 240.0, _animation));
    
    _player->process("camera", deltaTime);
	vmml::Matrix4f playerModel = _player->computeTransformationMatrix();
	vmml::Matrix4f playerView = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
	vmml::Matrix4f playerModelView = playerView * playerModel;
	bRenderer().getObjects()->getShader("basic")->setUniform("playerPos", playerView* _player->getPosition());
	bRenderer().getObjects()->getShader("terrain")->setUniform("playerPos", playerView* _player->getPosition());

    _terrainLoader->process("camera", deltaTime);


    
	/// TREE ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(100.0, 50.0, 0.0)) *
        vmml::create_rotation((float)elapsedTime * M_PI_F/10, vmml::Vector3f::UNIT_Y) *
        vmml::create_scaling(vmml::Vector3f(0.5f));
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("tree", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);

    /////// Skybox ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(_player->getPosition().x(), 0.0, _player->getPosition().z())) *
        vmml::create_scaling(vmml::Vector3f(1.0));
    // set CubeMap for skybox texturing
    skybox = bRenderer().getObjects()->getShader("skybox");
    skybox->setUniform("CubeMap", bRenderer().getObjects()->getCubeMap("skyBoxCubeMap"));
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("skybox", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);

	/// SUN ///
	_sun->render(camera, _player->getPosition());
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
