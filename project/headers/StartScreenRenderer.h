#ifndef B_START_SCREEN_RENDERER_H
#define B_START_SCREEN_RENDERER_H

#include "bRenderer.h"
#include "Cam.h"
#include "TerrainLoader.h"
#include "Player.h"
#include "PlayerCamera.h"

class StartScreenRenderer
{
public:
	StartScreenRenderer();

	StartScreenRenderer(Renderer & renderer, CamPtr camera, vmml::Matrix4f viewMatrixHUD);

	StartScreenRenderer(Renderer & renderer, CamPtr camera, TerrainLoaderPtr terrainLoader, vmml::Matrix4f viewMatrixHUD);

	void bindBlurFbo();

	void showStartScreen(vmml::Matrix4f modelMatrix);

    void showStartScreen(bool gameHasEnded);

	void startNewGame();

	bool isOverNewGameButton(double xpos, double ypos);
    
    bool newGameButtonPressed();
    
    void resetGameBoolean();

private:
	Renderer _renderer;
	GLint _defaultFBO;
	vmml::Matrix4f _viewMatrixHUD;
	bool _showScreen;
	CamPtr _camera;
	TerrainLoaderPtr _terrainLoader;
	
	bool isInScreenBounds(double xpos, int numeratorX, int denominatorX, double ypos, int numeratorY, int denominatorY);
	void handleNewGameButton(double xpos, double ypos);
    bool _startNewGame = false;
};
typedef std::shared_ptr< StartScreenRenderer >  StartScreenRendererPtr;
#endif /* defined(B_START_SCREEN_RENDERER_H) */
