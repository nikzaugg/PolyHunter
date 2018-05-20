#ifndef B_ShadowModelRenderer_H
#define B_ShadowModelRenderer_H

#include "bRenderer.h"
#include "Player.h"
#include "TerrainLoader.h"


class ShadowModelRenderer : public ModelRenderer {
    
public:
    ShadowModelRenderer(Renderer & renderer, PlayerPtr player, TerrainLoaderPtr terrainLoader) : ModelRenderer() {
        _renderer = renderer;
        _player = player;
        _terrainLoader = terrainLoader;
        _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    };
    
    void doShadowMapping(const double &deltaTime);
    
    vmml::Matrix4f getDepthMVP();
    vmml::Matrix4f getDepthView();
    vmml::Matrix4f getDepthProjection();
    vmml::Matrix4f getOffsetMatrix();

private:
    // setup new camera with orthogonal projection matrix
    // setup camera's view matrix (lookat)
    void setupCameraConfiguration();
    
    // create simple shader that is used by all shadow objects
    void setupShadowShader();
    
    // setup FBO-Depth Map
    void setupShadowFBO();
    
    // render scene with objects that cast shadows
    void renderShadowScene(const double &deltaTime);
    
    float toDegrees(float radian);
    
    float toRadians(float degree);
    
    Renderer _renderer;
    ShaderPtr _depthShader;
    FramebufferPtr _depthFBO;
    DepthMapPtr _depthMap;
    TexturePtr _depthTexture;
    MaterialPtr _depthMaterial;
    PlayerPtr _player;
    TerrainLoaderPtr _terrainLoader;
    
    vmml::Vector3f _lightPosition;
    vmml::Vector3f _invLightPosition;
    vmml::Matrix4f _depthProjectionMatrix;
    vmml::Matrix4f _depthViewMatrix;
    vmml::Matrix4f _depthModelMatrix;
    vmml::Matrix4f _depthMVP;
    vmml::Matrix4f _viewMatrixHUD;
    
};
typedef std::shared_ptr< ShadowModelRenderer >  ShadowModelRendererPtr;
#endif /* defined(B_ShadowModelRenderer_H) */

