#ifndef B_SSAORenderer_H
#define B_SSAORenderer_H

#include "bRenderer.h"
#include "TerrainLoader.h"

class BloomRenderer : public ModelRenderer {
    
public:
    BloomRenderer(Renderer & renderer, TerrainLoaderPtr terrainLoader) : ModelRenderer() {
        _renderer = renderer;
        _terrainLoader = terrainLoader;
        _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
        setup();
    };
    
    void doBloomRenderPass(std::string camera, const double &deltaTime);
    
private:
    
    void setup();
    void setupBloomShaders();
    void setupBloomFBO();
    
    Renderer _renderer;
    TerrainLoaderPtr _terrainLoader;
    
    // Create 3 different shaders for each processing step
    ShaderPtr _simpleTextureShader;
    ShaderPtr _blurShader;
    ShaderPtr _combinedShader;
    MaterialPtr _modelsToBlurMaterial;
    MaterialPtr _blurMaterial;
    MaterialPtr _combinedMaterial;
    FramebufferPtr _depthFBO;
    DepthMapPtr _depthMap;
    TexturePtr _depthTexture;
    

    vmml::Matrix4f _viewMatrixHUD;
    
};
typedef std::shared_ptr< SSAORenderer >  SSAORendererPtr;
#endif /* defined(B_SSAORenderer_H) */


