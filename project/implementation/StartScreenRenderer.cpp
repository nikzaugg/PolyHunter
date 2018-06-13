#include "StartScreenRenderer.h"
#include "TerrainLoader.h"

StartScreenRenderer::StartScreenRenderer(Renderer & renderer, CamPtr camera, TerrainLoaderPtr terrainLoader, vmml::Matrix4f viewMatrixHUD)
{
	_renderer = renderer;
	_viewMatrixHUD = viewMatrixHUD;
	_showScreen = true;
	_camera = camera;
	_terrainLoader = terrainLoader;

	// Create Blur Framebuffer Object
	_renderer.getObjects()->createFramebuffer("blurFbo");

	_renderer.getObjects()->createTexture("blurFbo_texture1", 0.0f, 0.0f);
	_renderer.getObjects()->createTexture("blurFbo_texture2", 0.0f, 0.0f);

	ShaderPtr blurShader = _renderer.getObjects()->loadShaderFile_o("blurShader", 0);
	MaterialPtr blurMaterial = _renderer.getObjects()->createMaterial("blurMaterial", blurShader);
	_renderer.getObjects()->createSprite("blurSprite", blurMaterial);

	_renderer.getObjects()->createSprite("blurSprite2", blurMaterial);

	FontPtr font = _renderer.getObjects()->loadFont("KozGoPro-ExtraLight.otf", 50);

	// Resume
	_renderer.getObjects()->createTextSprite("resume-text", vmml::Vector3f(1.f, 1.f, 1.f), " ", font);
	_renderer.getObjects()->getTextSprite("resume-text")->setText("Resume");
	
	// New Game
	_renderer.getObjects()->createTextSprite("new-game-text", vmml::Vector3f(1.f, 1.f, 1.f), " ", font);
	_renderer.getObjects()->getTextSprite("new-game-text")->setText("New Game");

	// Title
	_renderer.getObjects()->createTextSprite("title", vmml::Vector3f(1.f, 1.f, 1.f), " ", font);
	_renderer.getObjects()->getTextSprite("title")->setText("PolyHunter");
}

void StartScreenRenderer::bindBlurFbo()
{
	if (_showScreen)
	{
		_renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());		// reduce viewport size
		_defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
		_renderer.getObjects()->getFramebuffer("blurFbo")->bindTexture(_renderer.getObjects()->getTexture("blurFbo_texture1"), false);
	}
}

bool StartScreenRenderer::isInScreenBounds(double xpos, int numeratorX, int denominatorX, double ypos, int numeratorY, int denominatorY)
{
	int width = (int)_renderer.getView()->getWidth();
	int height = (int)_renderer.getView()->getHeight();

	bool isInXBounds = (numeratorX - 1) * (width / denominatorX) <= xpos && xpos <= numeratorX * (width / denominatorX);
	bool isInYBounds = (numeratorY - 1) * (height / denominatorY) <= ypos && ypos <= numeratorY * (width / denominatorY);

	return isInXBounds && isInYBounds;
}

void StartScreenRenderer::handleNewGameButton(double xpos, double ypos)
{
	if (isInScreenBounds(xpos, 3, 4, ypos, 2, 2)) {
		_renderer.getObjects()->getTextSprite("new-game-text")->setColor(vmml::Vector3f(0.11f, 0.29f, 0.42f));

		if (_renderer.getInput()->getMouseButtonState(bRenderer::LEFT_MOUSE_BUTTON))
		{
			startNewGame();
		}
	}
	else {
		_renderer.getObjects()->getTextSprite("new-game-text")->setColor(vmml::Vector3f(1.0f, 1.0f, 1.0f));
	}
}



void StartScreenRenderer::showStartScreen()
{
	if (_showScreen)
	{
		/*** Blur ***/
		_renderer.getObjects()->getFramebuffer("blurFbo")->bindTexture(_renderer.getObjects()->getTexture("blurFbo_texture2"), false);
		vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));

		_renderer.getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("blurFbo_texture1"));
		_renderer.getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(false));
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

		// translate
		modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
		// blur vertically and horizontally

		_renderer.getObjects()->getFramebuffer("blurFbo")->unbind(_defaultFBO); //unbind (original fbo will be bound)
		_renderer.getView()->setViewportSize(_renderer.getView()->getWidth(), _renderer.getView()->getHeight());								// reset vieport size

		_renderer.getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", _renderer.getObjects()->getTexture("blurFbo_texture2"));
		_renderer.getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(true));

		// draw
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("blurSprite2"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

		/*** GUI - Start Game Text ***/
		// Draw without blur
		GLfloat titleScale = 0.2f;
		vmml::Matrix4f scaling = vmml::create_scaling(vmml::Vector3f(titleScale / _renderer.getView()->getAspectRatio(), titleScale, titleScale));

		modelMatrix = vmml::create_translation(vmml::Vector3f(0.1f, 0.0f, -0.65f)) * scaling;
		// draw
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getModel("crystal_icon"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false, false);

		modelMatrix = vmml::create_translation(vmml::Vector3f(-0.23f, 0.4f, -0.65f)) * scaling;
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getTextSprite("title"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false, false);

		titleScale = 0.1f;
		scaling = vmml::create_scaling(vmml::Vector3f(titleScale / _renderer.getView()->getAspectRatio(), titleScale, titleScale));

		modelMatrix = vmml::create_translation(vmml::Vector3f(-0.1f, 0.0f, -0.65f)) * scaling;
		// draw
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getTextSprite("gui-crystal-info"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);


		modelMatrix = vmml::create_translation(vmml::Vector3f(-0.5f, -0.7f, -0.65f)) * scaling;
		// draw
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getTextSprite("resume-text"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

		modelMatrix = vmml::create_translation(vmml::Vector3f(0.2f, -0.7f, -0.65f)) * scaling;
		_renderer.getModelRenderer()->drawModel(_renderer.getObjects()->getTextSprite("new-game-text"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
		double xpos, ypos; bool hasCursor = false;
		_renderer.getInput()->getCursorPosition(&xpos, &ypos, &hasCursor);

		if (isInScreenBounds(xpos, 2, 4, ypos, 2, 2)) {
			_renderer.getObjects()->getTextSprite("resume-text")->setColor(vmml::Vector3f(0.11f, 0.29f, 0.42f));

			if (_renderer.getInput()->getMouseButtonState(bRenderer::LEFT_MOUSE_BUTTON))
			{
				_showScreen = false;
				_camera->setMovable(true);
				_renderer.getInput()->setCursorEnabled(false);
			}

		}
		else {
			_renderer.getObjects()->getTextSprite("resume-text")->setColor(vmml::Vector3f(1.0f, 1.0f, 1.0f));
		}

		handleNewGameButton(xpos, ypos);
	}
	else {
		// Case: Start Screen inactive
		if (_renderer.getInput()->getKeyState(bRenderer::KEY_SPACE))
		{
			// Show start screen and disable camera movement
			_showScreen = true;
			_camera->setMovable(false);
			_renderer.getInput()->setCursorEnabled(true);
		}
	}
}

void StartScreenRenderer::startNewGame()
{
	Terrain::seed = Terrain::getRandomSeed();
	_camera->setPosition(vmml::Vector3f(0.0f, Terrain::getHeightFromNoise(0.0, 0.0), 0.0f));
	_terrainLoader->reloadTerrains();
}