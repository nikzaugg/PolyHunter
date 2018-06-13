#ifndef B_START_SCREEN_RENDERER_H
#define B_START_SCREEN_RENDERER_H

#include "bRenderer.h"
#include "Cam.h"

class StartScreenRenderer
{
public:
	StartScreenRenderer();

	StartScreenRenderer(Renderer & renderer, CamPtr camera, vmml::Matrix4f viewMatrixHUD);

	void bindBlurFbo();

	void showStartScreen(vmml::Matrix4f modelMatrix);

	void showStartScreen();

private:
	Renderer _renderer;
	GLint _defaultFBO;
	vmml::Matrix4f _viewMatrixHUD;
	bool _showScreen;
	CamPtr _camera;
	
	bool isInScreenBounds(double xpos, int numeratorX, int denominatorX, double ypos, int numeratorY, int denominatorY);
};
typedef std::shared_ptr< StartScreenRenderer >  StartScreenRendererPtr;
#endif /* defined(B_START_SCREEN_RENDERER_H) */