# include "ShadowModelRenderer.h"
#include "Player.h"
#include "TerrainLoader.h"

// entry point of the ShadowModelRenderer
void ShadowModelRenderer::doShadowMapping(const double &deltaTime)
{
    // setup new camera with orthogonal projection matrix
    // setup camera's view matrix (lookat)
    setupCameraConfiguration();
    
    // create simple shader that is used by all shadow objects
    setupShadowShader();
    
    // setup FBO-Depth Map
    setupShadowFBO();
    
    // render scene with objects that cast shadows
    renderShadowScene(deltaTime);
}

// setup new camera with orthogonal projection matrix
// setup camera's view matrix (lookat)
void ShadowModelRenderer::setupCameraConfiguration()
{
    _lightPosition = _renderer.getObjects()->getLight("sun")->getPosition(); // (1000, 1000, 0)
    _invLightPosition = vmml::Vector3f(-_lightPosition.x(), -_lightPosition.y(), -_lightPosition.z());
    vmml::Vector3f playerPos = _player->getPosition();
    // ortho(0.0f, _renderer.getView()->getWidth(), _renderer.getView()->getHeight(), 0.0f, -1.0f, 1.0f, _depthProjectionMatrix);
    // void ShadowModelRenderer::ortho(const float &b, const float &t, const float &l, const float &r, const float &n, const float &f, vmml::Matrix4f &M)
//    ortho(
//          0.0, // bottom
//          _renderer.getView()->getHeight(), // top
//          0.0, // left
//          _renderer.getView()->getWidth(), // right
//          0.1, // near
//          1000, // far
//          _depthProjectionMatrix
//          );
    //_depthProjectionMatrix = _renderer.getObjects()->getCamera("camera")->getProjectionMatrix();
//    std::cout << "Ortho Proj Matrix: " << std::endl;
//    std::cout << _depthProjectionMatrix << std::endl;
//    std::cout << "------------------" << std::endl;
    
//    vmml::Matrix4f orthoMatrix = vmml::Matrix4f::IDENTITY;
//    orthoMatrix.at(0, 0) = 2.0 / _renderer.getView()->getWidth();
//    orthoMatrix.at(1, 1) = 2.0 / _renderer.getView()->getHeight();
//    orthoMatrix.at(2, 2) = -2.0 / 100000.0;
//    orthoMatrix.at(3, 3) = 1;
//    _depthProjectionMatrix = orthoMatrix;
    
    vmml::Matrix4f orthoMatrix = vmml::Matrix4f::IDENTITY;
    orthoMatrix.at(0, 0) = 2.0 / 300.0;
    orthoMatrix.at(1, 1) = 2.0 / 300.0;
    orthoMatrix.at(2, 2) = -2.0 / 1000.0;
    orthoMatrix.at(3, 3) = 1;
    _depthProjectionMatrix = orthoMatrix;
    
    vmml::Vector3f direction = vmml::Vector3f(-_lightPosition.x(), -_lightPosition.y(), -_lightPosition.z());
    direction = normalize(direction);
    vmml::Matrix4f lightViewMatrix = vmml::Matrix4f::IDENTITY;
    vmml::Vector3f center = playerPos;
    center = vmml::Vector3f(-playerPos.x(), -playerPos.y(), -playerPos.z());
    float pitchVectorLenght = sqrt( pow(direction.x(), 2.0) + pow(direction.z(), 2.0) );
    float pitch = (float)acos(pitchVectorLenght);
    lightViewMatrix *= vmml::create_rotation(pitch, vmml::Vector3f::UNIT_X);
    float yawRadians = (float)atan(direction.x()/direction.z());
    float yaw = yawRadians * (180.0/3.141592653589793238463);
    yaw = direction > 0 ? yaw - 180 : yaw;
    lightViewMatrix *= vmml::create_rotation((float)-1.0*yawRadians, vmml::Vector3f::UNIT_Y);
    lightViewMatrix *= vmml::create_translation(vmml::Vector3f(center));
    std::cout << "lightViewMatrix: " << lightViewMatrix << std::endl;
    _depthViewMatrix = lightViewMatrix;
    
//    _depthViewMatrix = Camera::lookAt(
//                                      vmml::Vector3f(0.0, -10.0, 10.0),
//                                      vmml::Vector3f(0.0),
//                                      vmml::Vector3f::UNIT_Y
//                                      );
//    _depthViewMatrix *= vmml::create_translation(vmml::Vector3f(0.0, -200.0, 0.0));
//    _depthViewMatrix *= vmml::create_rotation((float)(M_PI_F/2.0), vmml::Vector3f::UNIT_Y);
    // _depthViewMatrix = _renderer.getObjects()->getCamera("camera")->getViewMatrix();
    // _depthViewMatrix = _viewMatrixHUD;
    // std::cout << "Depth View Matrix: " << _depthViewMatrix << std::endl;
    
    
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
void ShadowModelRenderer::renderShadowScene(const double &deltaTime)
{
    /**************************
     * BIND DEPTH-FBO
     *************************/
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    GLint defaultFBO = Framebuffer::getCurrentFramebuffer();
    _renderer.getObjects()->getFramebuffer("depthFBO")->bindDepthMap(_renderer.getObjects()->getDepthMap("depthMap"), false);
    _renderer.getObjects()->getFramebuffer("depthFBO")->bindTexture(_renderer.getObjects()->getTexture("sceneTexture"), false);
    
    /********************************************
     * RENDER TERRAIN AND PLAYER TO DEPTH TEXTURE
     *******************************************/
//    _renderer.getObjects()->getCamera("depthCamera")->setPosition(vmml::Vector3f(-_player->getPosition().x(), -30.0, _player->getPosition().z()));
//    _renderer.getObjects()->getCamera("depthCamera")->setRotation(vmml::Vector3f(-M_PI_F/2.0, 0.0, 0.0));
    _player->customProcess("camera", deltaTime, _depthViewMatrix, _depthProjectionMatrix);
    _terrainLoader->customProcess("depthCamera", deltaTime, _depthViewMatrix, _depthProjectionMatrix);
    _renderer.getModelRenderer()->drawQueue(/*GL_LINES*/);
    _renderer.getModelRenderer()->clearQueue();
    /**********************************/

    /**************************************
     * RENDER DEPTH-MAP ONTO A GUI-SPRITE
     * to see it on screen
     *************************************/
    _renderer.getObjects()->getFramebuffer("shadowFBO")->bindTexture(_renderer.getObjects()->getTexture("shadowTexture"), false);
    vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getMaterial("depthMaterial")->setTexture("depthMap", _renderer.getObjects()->getDepthMap("depthMap"));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /**************************************
     * RENDER SCENE ONTO A GUI-SPRITE
     * to see it on screen
     *************************************/
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getMaterial("brightMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("sceneTexture"));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sceneSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /**********************************
     * RENDER TO DEFAULT FRAMEBUFFER  *
     * Switch to detault framebuffer (the screen) and draw the created spriteS
     *********************************/
    _renderer.getObjects()->getFramebuffer("shadowFBO")->unbind(defaultFBO); //unbind (original fbo will be bound)
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    
//    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("sceneSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

    modelMatrix = vmml::create_translation(vmml::Vector3f(-.75f, 0.75f, -0.5)) *
    vmml::create_scaling(vmml::Vector3f(0.25));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
}

// draws a model onto the shadow map
void ShadowModelRenderer::drawShadowModel(std::string ModelName, vmml::Matrix4f  &modelMatrix, vmml::Matrix4f &ViewMatrix, vmml::Matrix4f &ProjectionMatrix, const std::vector<std::string> &lightNames, bool doFrustumCulling, bool cullIndividualGeometry)
{
    drawModel(_renderer.getObjects()->getModel(ModelName), modelMatrix, ViewMatrix, ProjectionMatrix, lightNames, doFrustumCulling, cullIndividualGeometry);
}

// set the OpenGL orthographic projection matrix
void ShadowModelRenderer::ortho(const float &b, const float &t, const float &l, const float &r, const float &n, const float &f, vmml::Matrix4f &M)
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
