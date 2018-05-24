#ifndef B_ShadowModelRenderer_H
#define B_ShadowModelRenderer_H

#include "bRenderer.h"
#include "Cam.h"
#include "TerrainLoader.h"


class ShadowModelRenderer : public ModelRenderer {
    
public:
    ShadowModelRenderer(Renderer & renderer, CamPtr player, TerrainLoaderPtr terrainLoader) : ModelRenderer() {
        _renderer = renderer;
        _player = player;
        _terrainLoader = terrainLoader;
        _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
        setup();
    };
    
    void doShadowRenderPass(std::string shaderToSendUniformsTo, const double &deltaTime, const double &elapsedTime, bool debug = false);
    float getShadowBoxLength();
    vmml::Matrix4f getDepthMVP();
    vmml::Matrix4f getDepthView();
    vmml::Matrix4f getDepthProjection();
    vmml::Matrix4f getOffsetMatrix();

private:
    void doShadowMapping(const double &deltaTime);
    void doShadowMappingDebug(const double &deltaTime);
    
    void setup();
    void setupShadowShader();
    void setupShadowFBO();
    void createOrthogonalProjectionMatrix();
    void updateLightViewMatrix();
    
    // render scene with objects that cast shadows
    void drawToDepthMap(const double &deltaTime);
    // render scene with objects that cast shadows and display depth map in a GUI
    void drawToDepthMapDebug(const double &deltaTime);
    
    float toDegrees(float radian);
    float toRadians(float degree);
    
    Renderer _renderer;
    ShaderPtr _depthShader;
    FramebufferPtr _depthFBO;
    DepthMapPtr _depthMap;
    TexturePtr _depthTexture;
    MaterialPtr _depthMaterial;
    CamPtr _player;
    TerrainLoaderPtr _terrainLoader;
    
    vmml::Vector3f _lightPosition;
    vmml::Vector3f _invLightPosition;
    vmml::Matrix4f _depthProjectionMatrix;
    vmml::Matrix4f _depthViewMatrix;
    vmml::Matrix4f _depthModelMatrix;
    vmml::Matrix4f _depthMVP;
    vmml::Matrix4f _viewMatrixHUD;
    
    float _shadowBoxWidth = 350.0;
    float _shadowBoxHeight = 350.0;
    float _shadowBoxLength = 400.0;
    
};
typedef std::shared_ptr< ShadowModelRenderer >  ShadowModelRendererPtr;
#endif /* defined(B_ShadowModelRenderer_H) */

