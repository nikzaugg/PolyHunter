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
    // CREATE ORTHOGRAPHIC PROJECTION MATRIX
    vmml::Matrix4f orthoMatrix = vmml::Matrix4f::IDENTITY;
    orthoMatrix.at(0, 0) = 2.0 / -200;
    orthoMatrix.at(1, 1) = 2.0 / 200;
    orthoMatrix.at(2, 2) = -2.0 / 1000.0;
    orthoMatrix.at(3, 3) = 1;
    _depthProjectionMatrix = orthoMatrix;
    
    // CREATE LIGHT VIEW MATRIX
    vmml::Matrix4f lightViewMatrix = vmml::Matrix4f::IDENTITY;
    _lightPosition = _renderer.getObjects()->getLight("sun")->getPosition(); 
    _lightPosition = _lightPosition;
    vmml::Vector3f direction = vmml::Vector3f(-_lightPosition.x(), -_lightPosition.y(), -_lightPosition.z());
    // vmml::Vector3f direction = vmml::Vector3f(_lightPosition);
    direction = normalize(direction);

    vmml::Vector3f playerPos = _player->getPosition();
    vmml::Vector3f center = vmml::Vector3f(-playerPos.x(), -playerPos.y(), -playerPos.z());
    //vmml::Vector3f center = vmml::Vector3f(0.0);

    float pitchVectorLenght = sqrt( pow(direction.x(), 2.0) + pow(direction.z(), 2.0) );
    float pitch = (float)acos(pitchVectorLenght);
    std::cout << "pitch: " << pitch << std::endl;
    pitch = pitch;
    lightViewMatrix *= vmml::create_rotation(pitch, vmml::Vector3f::UNIT_X);

    float yaw = (float)toDegrees((float)atan(direction.x()/direction.z()));
    yaw = direction.z() > 0 ? (yaw - 180) : yaw;
    std::cout << "yaw: " << yaw << std::endl;
    lightViewMatrix *= vmml::create_rotation((float)-1.0*toRadians(yaw), vmml::Vector3f::UNIT_Y);
    lightViewMatrix *= vmml::create_translation(vmml::Vector3f(center));

    _depthViewMatrix = lightViewMatrix;

    std::cout << "LightViewMatrix: " << std::endl;
    std::cout << _depthViewMatrix << std::endl;
    std::cout << "--------------------- " << std::endl;
}

float ShadowModelRenderer::toDegrees(float radian){
    return ( radian * 180.0 ) / M_PI_F ;
}

float ShadowModelRenderer::toRadians(float degree){
    return ( degree * M_PI_F ) / 180.0 ;
}

// create simple shader that is used by all shadow objects
void ShadowModelRenderer::setupShadowShader(){
    _depthShader = _renderer.getObjects()->loadShaderFile_o("depthShader", 0);
    _depthShader = _renderer.getObjects()->loadShaderFile_o("simpleTexture", 0);
}

// setup FBO-Depth Map
void ShadowModelRenderer::setupShadowFBO()
{
    /******************
     Depth FBO
     *****************/
    _depthFBO = _renderer.getObjects()->createFramebuffer("depthFBO");                    // create framebuffer object
    _depthMap = _renderer.getObjects()->createDepthMap("depthMap", _renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    
    _depthMaterial = _renderer.getObjects()->createMaterial("depthMaterial", _renderer.getObjects()->getShader("simpleTexture"));
    _renderer.getObjects()->createSprite("depthSprite", _depthMaterial);
    _renderer.getObjects()->createSprite("depthSceneSprite", _depthMaterial);
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
    //_renderer.getObjects()->getFramebuffer("depthFBO")->bindTexture(_renderer.getObjects()->getTexture("sceneTexture"), false);
    
    /********************************************
     * RENDER TERRAIN AND PLAYER TO DEPTH TEXTURE
     *******************************************/
    _player->customProcess("camera", deltaTime, getDepthView(), getDepthProjection());
    _terrainLoader->customProcess("camera", deltaTime, getDepthView(), getDepthProjection());
    /**********************************/

    /**************************************
     * RENDER DEPTH-MAP ONTO A GUI-SPRITE
     * to see it on screen
     *************************************/
    _renderer.getObjects()->getFramebuffer("shadowFBO")->bindTexture(_renderer.getObjects()->getTexture("shadowTexture"), false);
    vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    // _renderer.getObjects()->getMaterial("depthMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("sceneTexture"));
    _renderer.getObjects()->getMaterial("depthMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getDepthMap("depthMap"));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /**************************************
     * RENDER SCENE ONTO A GUI-SPRITE
     * to see it on screen
     *************************************/
//    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
//    _renderer.getObjects()->getMaterial("depthMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("sceneTexture"));
//    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSceneSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /**********************************
     * RENDER TO DEFAULT FRAMEBUFFER  *
     * Switch to detault framebuffer (the screen) and draw the created spriteS
     *********************************/
    _renderer.getObjects()->getFramebuffer("shadowFBO")->unbind(defaultFBO); //unbind (original fbo will be bound)
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    
//    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSceneSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

    // modelMatrix = vmml::create_translation(vmml::Vector3f(-.75f, 0.75f, -0.5)) *
    modelMatrix = vmml::create_translation(vmml::Vector3f(-0.5f, 0.5f, -0.5)) *
    vmml::create_scaling(vmml::Vector3f(0.5));
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("depthSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
}

vmml::Matrix4f ShadowModelRenderer::getDepthMVP(){
    vmml::Matrix4f offset = vmml::Matrix4f::IDENTITY;
    offset *= vmml::create_translation(vmml::Vector3f(0.5));
    offset *= vmml::create_scaling(vmml::Vector3f(0.5));
    
    vmml::Matrix4f projectionViewMatrix = _depthProjectionMatrix * _depthViewMatrix;
    return offset * projectionViewMatrix;
}

vmml::Matrix4f ShadowModelRenderer::getDepthView(){
    return _depthViewMatrix;
}

vmml::Matrix4f ShadowModelRenderer::getDepthProjection(){
    return _depthProjectionMatrix;
}

vmml::Matrix4f ShadowModelRenderer::getOffsetMatrix(){
    vmml::Matrix4f offset = vmml::Matrix4f::IDENTITY;
    offset *= vmml::create_translation(vmml::Vector3f(0.5));
    offset *= vmml::create_scaling(vmml::Vector3f(0.5));
    
    return offset;
}

