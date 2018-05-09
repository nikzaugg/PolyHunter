#include "RenderProject.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "Skybox.h"
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
	ShaderPtr terrainShader = bRenderer().getObjects()->loadShaderFile("terrain", 1, false, true, true, true, false);
    ShaderPtr skyboxShader = bRenderer().getObjects()->loadShaderFile("skybox", 1, false, true, true, true, false);

	// PROPERTIES FOR THE MODELS
    PropertiesPtr treeProperties = bRenderer().getObjects()->createProperties("treeProperties");
    PropertiesPtr sunProperties = bRenderer().getObjects()->createProperties("sunProperties");
    PropertiesPtr skyboxProperties = bRenderer().getObjects()->createProperties("skyboxProperties");
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");

	// BLENDER MODELS (.obj)
    bRenderer().getObjects()->loadObjModel("tree.obj", false, true, basicShader, treeProperties);
    bRenderer().getObjects()->loadObjModel("sun.obj", false, true, basicShader, sunProperties);

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

    // create Player object
    _player = PlayerPtr(new Player("guy.obj", "guy", "guyProperties", basicShader, getProjectRenderer(), vmml::Vector3f(100.0, 0.0, 100.0), 0.0, -90.0, 0.0, 2.0));

    // PROCEDURAL TERRAIN TILES
    _terrainLoader = TerrainLoaderPtr(new TerrainLoader(getProjectRenderer(), terrainShader, _player));

	// create sprites
	bRenderer().getObjects()->createSprite("sparks", "sparks.png");										// create a sprite displaying sparks as a texture
	bRenderer().getObjects()->createSprite("bTitle", "basicTitle_light.png");							// create a sprite displaying the title as a texture

	// create camera
    bRenderer().getObjects()->createCamera("camera", vmml::Vector3f(0.0f, -100.0, 0.0), vmml::Vector3f(0.f, -M_PI_F/2.0, 0.f));
    _playerCamera = PlayerCameraPtr(new PlayerCamera("camera", _player, getProjectRenderer()));

	// create lights
	bRenderer().getObjects()->createLight("sun", vmml::Vector3f(0.0, 200.0, 0.0), vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 1.0f, 0.5f, 100.0f);

    // create text sprites
    FontPtr font = bRenderer().getObjects()->loadFont("KozGoPro-ExtraLight.otf", 50);
    if (Input::isTouchDevice())
        bRenderer().getObjects()->createTextSprite("instructions", vmml::Vector3f(1.f, 1.f, 1.f), "Double tap to start", font);
    else
        bRenderer().getObjects()->createTextSprite("instructions", vmml::Vector3f(1.f, 1.f, 1.f), "Press space to start", font);

	// postprocessing
	bRenderer().getObjects()->createFramebuffer("fbo");					// create framebuffer object
	bRenderer().getObjects()->createTexture("fbo_texture1", 0.f, 0.f);	// create texture to bind to the fbo
	bRenderer().getObjects()->createTexture("fbo_texture2", 0.f, 0.f);	// create texture to bind to the fbo
	//ShaderPtr blurShader = bRenderer().getObjects()->loadShaderFile("blurShader", 0, false, false, false, false, false);			// load shader that blurs the texture
	ShaderPtr blurShader = bRenderer().getObjects()->loadShaderFile_o("blurShader", 0);			// load shader that blurs the texture
	MaterialPtr blurMaterial = bRenderer().getObjects()->createMaterial("blurMaterial", blurShader);								// create an empty material to assign either texture1 or texture2 to
	bRenderer().getObjects()->createSprite("blurSprite", blurMaterial);																// create a sprite using the material created above

	// Update render queue
	updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
//	bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame

	//// Draw Scene and do post processing ////

	/// Begin post processing ///
	GLint defaultFBO;
	if (!_running){
		bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth() / 5, bRenderer().getView()->getHeight() / 5);		// reduce viewport size
		defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
		bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture1"), false);	// bind the fbo
	}

	/// Draw scene ///

	bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
	bRenderer().getModelRenderer()->clearQueue();

	if (!_running){
		/// End post processing ///
        /*** Blur ***/
		// translate
		vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
		// blur vertically and horizontally
		bool b = true;		int numberOfBlurSteps = 2;
		for (int i = 0; i < numberOfBlurSteps; i++) {
			if (i == numberOfBlurSteps - 1){
				bRenderer().getObjects()->getFramebuffer("fbo")->unbind(defaultFBO); //unbind (original fbo will be bound)
				bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
			}
			else
				bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture(b ? "fbo_texture2" : "fbo_texture1"), false);
			bRenderer().getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture(b ? "fbo_texture1" : "fbo_texture2"));
			bRenderer().getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(b));
			// draw
			bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
			b = !b;
		}

        /*** Title ***/
        // translate and scale
        GLfloat titleScale = 0.5f;
        vmml::Matrix4f scaling = vmml::create_scaling(vmml::Vector3f(titleScale / bRenderer().getView()->getAspectRatio(), titleScale, titleScale));
		modelMatrix = vmml::create_translation(vmml::Vector3f(-0.4f, 0.0f, -0.65f)) * scaling;
        // draw
		bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("bTitle"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false, false);

		/*** Instructions ***/
		titleScale = 0.1f;
		scaling = vmml::create_scaling(vmml::Vector3f(titleScale / bRenderer().getView()->getAspectRatio(), titleScale, titleScale));
		modelMatrix = vmml::create_translation(vmml::Vector3f(-0.45f / bRenderer().getView()->getAspectRatio(), -0.6f, -0.65f)) * scaling;
		// draw
		bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getTextSprite("instructions"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    }

	//// Camera Movement ////
	updateCamera("camera", deltaTime);

	/// Update render queue ///
	updateRenderQueue("camera", deltaTime);

    /// Update camera position according to player's position ///
    //updatePlayerCamera("camera", _player, deltaTime);
    _playerCamera->move();

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
    _terrainLoader->process();
    
    /// SUN ///
    modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 50.0, 0.0)) *
    vmml::create_rotation((float)elapsedTime * M_PI_F/10, vmml::Vector3f::UNIT_Y) *
    vmml::create_scaling(vmml::Vector3f(0.5f));
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("sun", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
    
	/// TREE ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(20.0, 50.0, 0.0)) *
        vmml::create_rotation((float)elapsedTime * M_PI_F/10, vmml::Vector3f::UNIT_Y) *
        vmml::create_scaling(vmml::Vector3f(0.5f));
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("tree", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);

    /// Skybox ///
    modelMatrix =
        vmml::create_translation(vmml::Vector3f(150.0, 100.0, 150.0)) *
        vmml::create_scaling(vmml::Vector3f(2.0));
    // set CubeMap for skybox texturing
    // skybox = bRenderer().getObjects()->getShader("skybox");
    // skybox->setUniform("CubeMap", bRenderer().getObjects()->getCubeMap("skyBoxCubeMap"));
    // set ambient color
    bRenderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("skybox", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
}

// Update camera position according to the players position
void RenderProject::updatePlayerCamera(const std::string &camera, PlayerPtr player, const double &deltaTime)
{
    vmml::Vector3f currentPlayerPosition = player->getPosition();
    bRenderer().getObjects()->getCamera("camera")->setPosition(
                                                               vmml::Vector3f(-currentPlayerPosition.x(),
                                                                              -currentPlayerPosition.y() - 20.0,
                                                                              -currentPlayerPosition.z() + 20.0));
    // TODO: camera should follow player more fluently etc.
}

/* Camera movement */
void RenderProject::updateCamera(const std::string &camera, const double &deltaTime)
{
	//// Adjust aspect ratio ////
	bRenderer().getObjects()->getCamera(camera)->setAspectRatio(bRenderer().getView()->getAspectRatio());

	double deltaCameraY = 0.0;
	double deltaCameraX = 0.0;
	double cameraForward = 0.0;
	double cameraSideward = 0.0;

    // pause using double tap
    if (bRenderer().getInput()->doubleTapRecognized()){
        _running = !_running;
    }

	/* iOS: control movement using touch screen */
//    if (Input::isTouchDevice()){
//
//        // pause using double tap
//        if (bRenderer().getInput()->doubleTapRecognized()){
//            _running = !_running;
//        }
//
//        if (_running){
//            // control using touch
//            TouchMap touchMap = bRenderer().getInput()->getTouches();
//            int i = 0;
//            for (auto t = touchMap.begin(); t != touchMap.end(); ++t)
//            {
//                Touch touch = t->second;
//                // If touch is in left half of the view: move around
//                if (touch.startPositionX < bRenderer().getView()->getWidth() / 2){
//                    cameraForward = -(touch.currentPositionY - touch.startPositionY) / 50;
//                    cameraSideward = (touch.currentPositionX - touch.startPositionX) / 50;
//
//                }
//                // if touch is in right half of the view: look around
//                else
//                {
//                    deltaCameraY = (touch.currentPositionX - touch.startPositionX) / 2000;
//                    deltaCameraX = (touch.currentPositionY - touch.startPositionY) / 2000;
//                }
//                if (++i > 2)
//                    break;
//            }
//        }
//
//    }
//    /* Windows: control movement using mouse and keyboard */
//    else{
//        // use space to pause and unpause
//        GLint currentStateSpaceKey = bRenderer().getInput()->getKeyState(bRenderer::KEY_SPACE);
//        if (currentStateSpaceKey != _lastStateSpaceKey)
//        {
//            _lastStateSpaceKey = currentStateSpaceKey;
//            if (currentStateSpaceKey == bRenderer::INPUT_PRESS){
//                _running = !_running;
//                bRenderer().getInput()->setCursorEnabled(!_running);
//            }
//        }
//
//        // mouse look
//        double xpos, ypos; bool hasCursor = false;
//        bRenderer().getInput()->getCursorPosition(&xpos, &ypos, &hasCursor);
//
//        deltaCameraY = (xpos - _mouseX) / 1000;
//        _mouseX = xpos;
//        deltaCameraX = (ypos - _mouseY) / 1000;
//        _mouseY = ypos;
//
//        if (_running){
//            // movement using wasd keys
//            if (bRenderer().getInput()->getKeyState(bRenderer::KEY_W) == bRenderer::INPUT_PRESS)
//                if (bRenderer().getInput()->getKeyState(bRenderer::KEY_LEFT_SHIFT) == bRenderer::INPUT_PRESS)             cameraForward = 2.0;
//                else            cameraForward = 1.0;
//            else if (bRenderer().getInput()->getKeyState(bRenderer::KEY_S) == bRenderer::INPUT_PRESS)
//                if (bRenderer().getInput()->getKeyState(bRenderer::KEY_LEFT_SHIFT) == bRenderer::INPUT_PRESS)             cameraForward = -2.0;
//                else            cameraForward = -1.0;
//            else
//                cameraForward = 0.0;
//
//            if (bRenderer().getInput()->getKeyState(bRenderer::KEY_A) == bRenderer::INPUT_PRESS)
//                cameraSideward = -1.0;
//            else if (bRenderer().getInput()->getKeyState(bRenderer::KEY_D) == bRenderer::INPUT_PRESS)
//                cameraSideward = 1.0;
//            if (bRenderer().getInput()->getKeyState(bRenderer::KEY_UP) == bRenderer::INPUT_PRESS)
//                bRenderer().getObjects()->getCamera(camera)->moveCameraUpward(_cameraSpeed*deltaTime);
//            else if (bRenderer().getInput()->getKeyState(bRenderer::KEY_DOWN) == bRenderer::INPUT_PRESS)
//                bRenderer().getObjects()->getCamera(camera)->moveCameraUpward(-_cameraSpeed*deltaTime);
//            if (bRenderer().getInput()->getKeyState(bRenderer::KEY_LEFT) == bRenderer::INPUT_PRESS)
//                bRenderer().getObjects()->getCamera(camera)->rotateCamera(0.0f, 0.0f, 0.03f*_cameraSpeed*deltaTime);
//            else if (bRenderer().getInput()->getKeyState(bRenderer::KEY_RIGHT) == bRenderer::INPUT_PRESS)
//                bRenderer().getObjects()->getCamera(camera)->rotateCamera(0.0f, 0.0f, -0.03f*_cameraSpeed*deltaTime);
//        }
//    }
//
//    //// Update camera ////
//    if (_running){
//        bRenderer().getObjects()->getCamera(camera)->moveCameraForward(cameraForward*_cameraSpeed*deltaTime);
//        bRenderer().getObjects()->getCamera(camera)->rotateCamera(deltaCameraX, deltaCameraY, 0.0f);
//        bRenderer().getObjects()->getCamera(camera)->moveCameraSideward(cameraSideward*_cameraSpeed*deltaTime);
//    }
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
