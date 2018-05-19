#ifndef B_ShadowModelRenderer_H
#define B_ShadowModelRenderer_H

#include "bRenderer.h"
#include "Player.h"


class ShadowModelRenderer : public ModelRenderer {
    
public:
    ShadowModelRenderer(Renderer & renderer, PlayerPtr player) : ModelRenderer() {
        _renderer = renderer;
        _player = player;
        _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    };
    
    void doShadowMapping();

private:
    void ortho(const float &b, const float &t, const float &l, const float &r,const float &n, const float &f,vmml::Matrix4f &M);
    
    // setup new camera with orthogonal projection matrix
    // setup camera's view matrix (lookat)
    void setupCameraConfiguration();
    
    // create simple shader that is used by all shadow objects
    void setupShadowShader();
    
    // setup FBO-Depth Map
    void setupShadowFBO();
    
    // render scene with objects that cast shadows
    void renderShadowScene();
    
    void drawShadowModel(std::string ModelName, vmml::Matrix4f  &modelMatrix, vmml::Matrix4f &ViewMatrix, vmml::Matrix4f &ProjectionMatrix, const std::vector<std::string> &lightNames, bool doFrustumCulling, bool cullIndividualGeometry);
    
    Renderer _renderer;
    ShaderPtr _depthShader;
    FramebufferPtr _depthFBO;
    DepthMapPtr _depthMap;
    TexturePtr _depthTexture;
    MaterialPtr _depthMaterial;
    PlayerPtr _player;
    
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

