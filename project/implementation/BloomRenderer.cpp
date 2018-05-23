
#include "BloomRenderer.h"

void BloomRenderer::setup()
{
    setupBloomShaders();
    
    setupBloomFBO();
}

void BloomRenderer::setupBloomShaders()
{
    // Create 3 different shaders for each processing step
    _simpleTextureShader = _renderer.getObjects()->loadShaderFile_o("simpleTexture", 0);
    _blurShader = _renderer.getObjects()->loadShaderFile_o("blurShader", 0);
    _combinedShader = _renderer.getObjects()->loadShaderFile_o("combineShader", 0);
}

void BloomRenderer::setupBloomFBO()
{
    // Create an FBO with textures
    _renderer.getObjects()->createFramebuffer("bloomFBO");                    // create framebuffer object
    _renderer.getObjects()->createTexture("sceneTexture", 0.f, 0.f);    // create texture to bind to the fbo
    _renderer.getObjects()->createTexture("crystalTexture", 0.f, 0.f);    // create texture to bind to the fbo
    _renderer.getObjects()->createTexture("modelsToBlurTexture", 0.f, 0.f);    // create texture to bind to the fbo
    _renderer.getObjects()->createTexture("horizBlurTexture", 0.f, 0.f);    // create texture to bind to the fbo
    _renderer.getObjects()->createTexture("vertBlurTexture", 0.f, 0.f);    // create texture to bind to the fbo
    _renderer.getObjects()->createTexture("combinedTexture", 0.f, 0.f);    // create texture to bind to the fbo
    
    // create 2 depth buffers
    _renderer.getObjects()->createDepthMap("sceneDepthMap", _renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    _renderer.getObjects()->createDepthMap("modelsToBlurDepthMap", _renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    
    // Create Materials for each processing step, so that we can pass a texture to the shader
    _modelsToBlurMaterial = _renderer.getObjects()->createMaterial("modelsToBlurMaterial", _simpleTextureShader);
    _blurMaterial = _renderer.getObjects()->createMaterial("blurMaterial", _blurShader);
    _combinedMaterial = _renderer.getObjects()->createMaterial("combinedMaterial", _combinedShader);
    
    // Create 3 Sprites which are rendered to an FBO
    _renderer.getObjects()->createSprite("sceneSprite", _modelsToBlurMaterial);
    _renderer.getObjects()->createSprite("modelsToBlurSprite", _modelsToBlurMaterial);
    _renderer.getObjects()->createSprite("blurSprite", _blurMaterial);
    _renderer.getObjects()->createSprite("combinedSprite", _combinedMaterial);
}

void BloomRenderer::doBloomRenderPass(std::string camera, const double &deltaTime)
{
    GLint defaultFBO;
    /*************************
     * BLOOM EFFECT BEGIN  *
     ************************/
    // bind Bloom FBO
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    defaultFBO = Framebuffer::getCurrentFramebuffer();    // get current fbo to bind it again after drawing the scene
    
    /// Render to Color Texture Attachment
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindTexture(_renderer.getObjects()->getTexture("sceneTexture"), false);    // bind the fbo
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindDepthMap(_renderer.getObjects()->getDepthMap("sceneDepthMap"), false);    // bind the fbo
    
    /// RENDER SCENE TO SCENE TEXTURE///
    /// DRAW MODELS THAT SHOULD HAVE THE BLOOM EFFECT ///
    _terrainLoader->process(camera, deltaTime);
    
    _renderer.getModelRenderer()->drawQueue(/*GL_LINES*/);
    _renderer.getModelRenderer()->clearQueue();
    
    /// Render to Color Texture Attachment
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindTexture(_renderer.getObjects()->getTexture("crystalTexture"), false);    // bind the fbo
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindDepthMap(_renderer.getObjects()->getDepthMap("modelsToBlurDepthMap"), false);    // bind the fbo
    
    /// DRAW MODELS THAT SHOULD HAVE THE BLOOM EFFECT ///
    vmml::Matrix4f modelMatrix =
    vmml::create_translation(vmml::Vector3f(0.0, 20.0, 0.0)) *
    vmml::create_scaling(vmml::Vector3f(5.0f));
    // draw model
    _renderer.getModelRenderer()->drawModel("Crystal", "camera", modelMatrix, std::vector<std::string>({ "sun" }), true, true);
    
    /// Render the created texture to another FBO, applying a bright filter (modelsToBlurTexture)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindTexture(_renderer.getObjects()->getTexture("modelsToBlurTexture"), false);    // bind the fbo
    _renderer.getObjects()->getMaterial("modelsToBlurMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("crystalTexture"));
    
    // draw currently active framebuffer
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("modelsToBlurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /// Render modelsToBlurTexture to another FBO, applying Gaussian Blur  HORIZONTAL (blurredTexture)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindTexture(_renderer.getObjects()->getTexture("horizBlurTexture"), false);    // bind the fbo
    _renderer.getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("modelsToBlurTexture"));
    _renderer.getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(true));
    // draw currently active framebuffer
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /// Render modelsToBlurTexture to another FBO, applying Gaussian Blur VERTICAL (blurredTexture)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindTexture(_renderer.getObjects()->getTexture("vertBlurTexture"), false);    // bind the fbo
    _renderer.getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("horizBlurTexture"));
    _renderer.getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(false));
    // draw currently active framebuffer
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /// Combine sceneTexture & blurredTexture and render to default FBO (screen)
    modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    _renderer.getObjects()->getFramebuffer("bloomFBO")->bindTexture(_renderer.getObjects()->getTexture("combinedTexture"), false);    // bind the fbo
    _renderer.getObjects()->getMaterial("combinedMaterial")->setTexture("fbo_texture1", _renderer.getObjects()->getTexture("sceneTexture"));
    _renderer.getObjects()->getMaterial("combinedMaterial")->setTexture("fbo_texture2", _renderer.getObjects()->getTexture("vertBlurTexture"));
    _renderer.getObjects()->getMaterial("combinedMaterial")->setTexture("fbo_depth1", _renderer.getObjects()->getDepthMap("sceneDepthMap"));
    _renderer.getObjects()->getMaterial("combinedMaterial")->setTexture("fbo_depth2", _renderer.getObjects()->getDepthMap("modelsToBlurDepthMap"));
    // draw currently active framebuffer
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("combinedSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /**********************************
     * RENDER TO DEFAULT FRAMEBUFFER  *
     * Switch to detault framebuffer (the screen) and draw the created sprite
     *********************************/
    _renderer.getObjects()->getFramebuffer("bloomFBO")->unbind(defaultFBO); //unbind (original fbo will be bound)
    _renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());
    
    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("combinedSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    // draw GUI element to show postprocessing
    //    modelMatrix = vmml::create_translation(vmml::Vector3f(-.75f, 0.75f, -0.5)) *
    //    vmml::create_scaling(vmml::Vector3f(0.25));
    //    _renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
}