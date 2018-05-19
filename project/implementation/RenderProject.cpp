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

	// create camera
    bRenderer().getObjects()->createCamera("camera",
                                           bRenderer::DEFAULT_FIELD_OF_VIEW(),
                                           bRenderer::DEFAULT_ASPECT_RATIO(),
                                           -1.0,
                                           1.0
                                           );
//    bRenderer().getObjects()->createCamera("camera");
    _playerCamera = PlayerCameraPtr(new PlayerCamera("camera", _player, getProjectRenderer()));
    std::cout << "ProjectionMatrix Camera" << std::endl;
    std::cout << bRenderer().getObjects()->getCamera("camera")->getProjectionMatrix() << std::endl;

	// create lights
     bRenderer().getObjects()->createLight("sun", vmml::Vector3f(100, 100, 0.0), vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 0.8f, 1.0f, 100.f);

//    /******************
//     Depth FBO
//     *****************/
//    bRenderer().getObjects()->createFramebuffer("depthFBO");                    // create framebuffer object
//    bRenderer().getObjects()->createDepthMap("depthMap", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
//    ShaderPtr depthShader = bRenderer().getObjects()->loadShaderFile_o("depthShader", 0);
//    MaterialPtr depthMaterial = bRenderer().getObjects()->createMaterial("depthMaterial", depthShader);
//    bRenderer().getObjects()->createSprite("depthSprite", depthMaterial);
//
//    /******************
//     SHADOW FBO
//     *****************/
    bRenderer().getObjects()->createFramebuffer("shadowFBO");                    // create framebuffer object
    bRenderer().getObjects()->createTexture("shadowTexture", 0.f, 0.f);    // create texture to bind to the fbo

    
    /******************
     FBO
     *****************/
    // Create an FBO with textures
    bRenderer().getObjects()->createFramebuffer("bloomFBO");                    // create framebuffer object
    bRenderer().getObjects()->createTexture("sceneTexture", 0.f, 0.f);    // create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("crystalTexture", 0.f, 0.f);    // create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("brightTexture", 0.f, 0.f);    // create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("horizBlurTexture", 0.f, 0.f);    // create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("vertBlurTexture", 0.f, 0.f);    // create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("combineTexture", 0.f, 0.f);    // create texture to bind to the fbo
    
    // create 2 depth buffers
    bRenderer().getObjects()->createDepthMap("sceneDepthMap", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    bRenderer().getObjects()->createDepthMap("crystalDepthMap", bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());

    // Create 3 different shaders for each processing step
    ShaderPtr brightFilterShader = bRenderer().getObjects()->loadShaderFile_o("brightShader", 0);
    ShaderPtr blurShader = bRenderer().getObjects()->loadShaderFile_o("blurShader", 0);
    ShaderPtr combineShader = bRenderer().getObjects()->loadShaderFile_o("combineShader", 0);
    
    // Create Materials for each processing step, so that we can pass a texture to the shader
    MaterialPtr brightMaterial = bRenderer().getObjects()->createMaterial("brightMaterial", brightFilterShader);
    MaterialPtr blurMaterial = bRenderer().getObjects()->createMaterial("blurMaterial", blurShader);
    MaterialPtr combineMaterial = bRenderer().getObjects()->createMaterial("combineMaterial", combineShader);

    // Create 3 Sprites which are rendered to an FBO (thus creating the texture)
    bRenderer().getObjects()->createSprite("sceneSprite", brightMaterial);
    bRenderer().getObjects()->createSprite("brightSprite", brightMaterial);
    bRenderer().getObjects()->createSprite("blurSprite", blurMaterial);
    bRenderer().getObjects()->createSprite("combineSprite", combineMaterial);
    
	// Update render queue
	updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
	// bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    // std::cout << "FPS: " << std::to_string(1 / deltaTime) << std::endl;
    doShadowMapping(deltaTime, elapsedTime);
    // doPostProcessingBloom(deltaTime, elapsedTime);
    _playerCamera->move();
    updateRenderQueue("camera", deltaTime);
    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
    bRenderer().getModelRenderer()->clearQueue();

    // updateCamera("camera", deltaTime);
    
	// Quit renderer when escape is pressed
	if (bRenderer().getInput()->getKeyState(bRenderer::KEY_ESCAPE) == bRenderer::INPUT_PRESS)
		bRenderer().terminateRenderer();
}

void RenderProject::doShadowMapping(const double &deltaTime, const double &elapsedTime){
    
    ShadowModelRendererPtr shadowModelRenderer = ShadowModelRendererPtr(new ShadowModelRenderer(getProjectRenderer(), _player));
    shadowModelRenderer->doShadowMapping();
    
}

void RenderProject::doPostProcessingBloom(const double &deltaTime, const double &elapsedTime){

    GLint defaultFBO;
    /*************************
     * BLOOM EFFECT BEGIN  *
     ************************/
    // bind Bloom FBO
    bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());        // reduce viewport size
    defaultFBO = Framebuffer::getCurrentFramebuffer();    // get current fbo to bind it again after drawing the scene
    
    /// Render to Color Texture Attachment
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("sceneTexture"), false);    // bind the fbo
    // render whole scene to an FBO (sceneTexture)
    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
    bRenderer().getModelRenderer()->clearQueue();
    updateRenderQueue("camera", deltaTime);
    updateCamera("camera", deltaTime);
    _playerCamera->move();
    
    /// Render to Depth Texture Attachment
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindDepthMap(bRenderer().getObjects()->getDepthMap("sceneDepthMap"), false);    // bind the fbo
    // render whole scene to an FBO (sceneTexture)
    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
    bRenderer().getModelRenderer()->clearQueue();
    updateRenderQueue("camera", deltaTime);
    updateCamera("camera", deltaTime);
    _playerCamera->move();
    
    /// Render to Color Texture Attachment
    // render crystal to an FBO (crystalTexture)
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("crystalTexture"), false);    // bind the fbo
    /// LOW POLY CRYSTAL ///
    vmml::Matrix4f modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 20.0, 0.0)) *
    vmml::create_scaling(vmml::Vector3f(5.0f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("Crystal", "camera", modelMatrix, std::vector<std::string>({ "sun" }), true, true);
    
    /// Render to Depth Texture Attachment
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindDepthMap(bRenderer().getObjects()->getDepthMap("crystalDepthMap"), false);    // bind the fbo
    /// LOW POLY CRYSTAL ///
    modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 20.0, 0.0)) *
    vmml::create_scaling(vmml::Vector3f(5.0f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("Crystal", "camera", modelMatrix, std::vector<std::string>({ "sun" }), true, true);
    
    /// Render the created texture to another FBO, applying a bright filter (brightTexture)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("brightTexture"), false);    // bind the fbo
    bRenderer().getObjects()->getMaterial("brightMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("crystalTexture"));
    
    // bRenderer().getObjects()->getMaterial("bloomMaterial")->setScalar("isVertical", static_cast<GLfloat>(true));
    // draw currently active framebuffer
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("brightSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /// Render brightTexture to another FBO, applying Gaussian Blur  HORIZONTAL (blurredTexture)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("horizBlurTexture"), false);    // bind the fbo
    bRenderer().getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("brightTexture"));
    bRenderer().getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(true));
    // draw currently active framebuffer
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /// Render brightTexture to another FBO, applying Gaussian Blur VERTICAL (blurredTexture)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("vertBlurTexture"), false);    // bind the fbo
    bRenderer().getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("horizBlurTexture"));
    bRenderer().getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(false));
    // draw currently active framebuffer
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /// Combine sceneTexture & blurredTexture and render to default FBO (screen)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->bindTexture(bRenderer().getObjects()->getTexture("combineTexture"), false);    // bind the fbo
    bRenderer().getObjects()->getMaterial("combineMaterial")->setTexture("fbo_texture1", bRenderer().getObjects()->getTexture("sceneTexture"));
    bRenderer().getObjects()->getMaterial("combineMaterial")->setTexture("fbo_texture2", bRenderer().getObjects()->getTexture("vertBlurTexture"));
    bRenderer().getObjects()->getMaterial("combineMaterial")->setTexture("fbo_depth1", bRenderer().getObjects()->getDepthMap("sceneDepthMap"));
    bRenderer().getObjects()->getMaterial("combineMaterial")->setTexture("fbo_depth2", bRenderer().getObjects()->getDepthMap("crystalDepthMap"));
    // draw currently active framebuffer
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("combineSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

    /**********************************
     * RENDER TO DEFAULT FRAMEBUFFER  *
     * Switch to detault framebuffer (the screen) and draw the created sprite
     *********************************/
    bRenderer().getObjects()->getFramebuffer("bloomFBO")->unbind(defaultFBO); //unbind (original fbo will be bound)
    bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());
    
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("combineSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    // draw GUI element to show postprocessing 
    modelMatrix = vmml::create_translation(vmml::Vector3f(-.75f, 0.75f, -0.5)) *
    vmml::create_scaling(vmml::Vector3f(0.25));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
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
	ShaderPtr skybox;
    
    _player->process(camera, deltaTime);
    _terrainLoader->process(camera, deltaTime);

    /// LOW POLY CRYSTAL ///
    modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 20.0, 0.0)) *
    vmml::create_scaling(vmml::Vector3f(5.0f));
    // draw model
    bRenderer().getModelRenderer()->drawModel("Crystal", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
    
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

    ///// Skybox ///
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

