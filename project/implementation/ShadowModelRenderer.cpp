# include "ShadowModelRenderer.h"
#include "Player.h"

// entry point of the ShadowModelRenderer
void ShadowModelRenderer::doShadowMapping()
{
    // setup new camera with orthogonal projection matrix
    // setup camera's view matrix (lookat)
    setupCameraConfiguration();
    
    // create simple shader that is used by all shadow objects
    setupShadowShader();
    
    // setup FBO-Depth Map
    setupShadowFBO();
    
    // render scene with objects that cast shadows
    renderShadowScene();
}

// setup new camera with orthogonal projection matrix
// setup camera's view matrix (lookat)
void ShadowModelRenderer::setupCameraConfiguration()
{
    _lightPosition = _renderer.getObjects()->getLight("sun")->getPosition();
    _invLightPosition = vmml::Vector3f(-_lightPosition.x(), -_lightPosition.y(), -_lightPosition.z());
    ortho(-10,10,-10,10,-10,20, _depthProjectionMatrix);
    _depthViewMatrix = Camera::lookAt(_invLightPosition, vmml::Vector3f(0.0f), vmml::Vector3f::UP);
    
    _renderer.getObjects()->createCamera("depthCamera");
    _renderer.getObjects()->getCamera("depthCamera")->setPosition(vmml::Vector3f(_player->getPosition().x(), -100.0, _player->getPosition().z()));
    _renderer.getObjects()->getCamera("depthCamera")->setRotation(vmml::Vector3f(-M_PI_F/2.0, 0.0, 0.0));
}

// create simple shader that is used by all shadow objects
void ShadowModelRenderer::setupShadowShader(){
    _depthShader = _renderer.getObjects()->loadShaderFile_o("depthShader", 0);
}

// setup FBO-Depth Map
void ShadowModelRenderer::setupShadowFBO()
{
    /******************
     Depth FBO
     *****************/
    _depthFBO = _renderer.getObjects()->createFramebuffer("depthFBO");                    // create framebuffer object
    _depthMap = _renderer.getObjects()->createDepthMap("depthMap", _renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    _depthMaterial = _renderer.getObjects()->createMaterial("depthMaterial", _renderer.getObjects()->getShader("depthShader"));
    _renderer.getObjects()->createSprite("depthSprite", _depthMaterial);
    _renderer.getObjects()->createTexture("depthTexture", 0.f, 0.f);    // create texture to bind to the fbo
}

// render scene with objects that cast shadows
void ShadowModelRenderer::renderShadowScene()
{
    /**************************
     * BIND DEPTH-FBO
     *************************/
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    GLint defaultFBO = Framebuffer::getCurrentFramebuffer();
    _renderer.getObjects()->getFramebuffer("depthFBO")->bindDepthMap(_renderer.getObjects()->getDepthMap("depthMap"), false);
    _renderer.getObjects()->getFramebuffer("depthFBO")->bindTexture(_renderer.getObjects()->getTexture("sceneTexture"), false);
    
    /**********************************
     * RENDER OBJECTS TO DEPTH TEXTURE
     *********************************/
    vmml::Matrix4f modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 0.0, 0.0)) *
    vmml::create_scaling(vmml::Vector3f(0.5f));
    CameraPtr depthCam = _renderer.getObjects()->getCamera("depthCamera");
    CameraPtr cam = _renderer.getObjects()->getCamera("camera");
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sun"), modelMatrix, depthCam->getViewMatrix(), depthCam->getProjectionMatrix(), std::vector<std::string>({}), true, true);
    
    modelMatrix =
    vmml::create_translation(vmml::Vector3f(5.0, 0.0, 5.0)) *
    vmml::create_scaling(vmml::Vector3f(0.5f));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sun"), modelMatrix, depthCam->getViewMatrix(), depthCam->getProjectionMatrix(), std::vector<std::string>({}), true, true);
    
    modelMatrix =
    vmml::create_translation(vmml::Vector3f(10.0, 0.0, 10.0)) *
    vmml::create_scaling(vmml::Vector3f(0.5f));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sun"), modelMatrix, depthCam->getViewMatrix(), depthCam->getProjectionMatrix(), std::vector<std::string>({}), true, true);

    /**************************************
     * RENDER DEPTH-MAP ONTO A GUI-SPRITE
     * to see it on screen
     *************************************/
    _renderer.getObjects()->getFramebuffer("shadowFBO")->bindTexture(_renderer.getObjects()->getTexture("shadowTexture"), false);
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getMaterial("depthMaterial")->setTexture("depthMap", _renderer.getObjects()->getDepthMap("depthMap"));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /**************************************
     * RENDER SCENE ONTO A GUI-SPRITE
     * to see it on screen
     *************************************/
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getMaterial("brightMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("sceneTexture"));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sceneSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    // pass shadowTexture to next Shader
    // pass sceneTexture to next Shader
    
    /**********************************
     * RENDER TO DEFAULT FRAMEBUFFER  *
     * Switch to detault framebuffer (the screen) and draw the created spriteS
     *********************************/
    _renderer.getObjects()->getFramebuffer("shadowFBO")->unbind(defaultFBO); //unbind (original fbo will be bound)
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    
//    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sceneSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
//
    modelMatrix = vmml::create_translation(vmml::Vector3f(-.75f, 0.75f, -0.5)) *
    vmml::create_scaling(vmml::Vector3f(0.25));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
}

void ShadowModelRenderer::drawShadowModel(std::string ModelName, vmml::Matrix4f  &modelMatrix, vmml::Matrix4f &ViewMatrix, vmml::Matrix4f &ProjectionMatrix, const std::vector<std::string> &lightNames, bool doFrustumCulling, bool cullIndividualGeometry)
{
    drawModel(_renderer.getObjects()->getModel(ModelName), modelMatrix, ViewMatrix, ProjectionMatrix, lightNames, doFrustumCulling, cullIndividualGeometry);
}



// set the OpenGL orthographic projection matrix
void ShadowModelRenderer::ortho(
                          const float &b, const float &t, const float &l, const float &r,
                          const float &n, const float &f,
                          vmml::Matrix4f &M)
{
    // set OpenGL perspective projection matrix
    M[0][0] = 2 / (r - l);
    M[0][1] = 0;
    M[0][2] = 0;
    M[0][3] = 0;
    
    M[1][0] = 0;
    M[1][1] = 2 / (t - b);
    M[1][2] = 0;
    M[1][3] = 0;
    
    M[2][0] = 0;
    M[2][1] = 0;
    M[2][2] = -2 / (f - n);
    M[2][3] = 0;
    
    M[3][0] = -(r + l) / (r - l);
    M[3][1] = -(t + b) / (t - b);
    M[3][2] = -(f + n) / (f - n);
    M[3][3] = 1;
}
