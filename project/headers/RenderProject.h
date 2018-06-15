#ifndef PROJECT_MAIN_H
#define PROJECT_MAIN_H

#include "bRenderer.h"
#include "Player.h"
#include "Terrain.h"
#include "TerrainLoader.h"
#include "PlayerCamera.h"
#include "ShadowModelRenderer.h"
#include "BloomRenderer.h"
#include "Cam.h"
#include "Sun.h"
#include "Skydome.h"
#include "StartScreenRenderer.h"

class RenderProject : public IRenderProject
{
public:
	/* Constructor and Destructor */
	RenderProject() : IRenderProject(){}
    virtual ~RenderProject(){bRenderer::log("RenderProject deleted");}

    /* Initialize the Project */
    void init();
    
	//// RenderProject functions ////

	/* This function is executed when initializing the renderer */
	void initFunction();

	/* Draw your scene here */
	void loopFunction(const double &deltaTime, const double &elapsedTime);

	/* This function is executed when terminating the renderer */
	void terminateFunction();


	//// iOS specific ////

    /* For iOS only: Handle device rotation */
    void deviceRotated();
    
    /* For iOS only: Handle app going into background */
    void appWillResignActive();
    
    /* For iOS only: Handle app coming back from background */
    void appDidBecomeActive();
    
    /* For iOS only: Handle app being terminated */
    void appWillTerminate();
    
	/* Make renderer accessible to public so we can get the UIView on iOS */
    Renderer& getProjectRenderer()
    {
        return bRenderer();
    }
    
private:
    
	/* Update render queue */
	void updateRenderQueue(const std::string &camera, const double &deltaTime);

	/* Camera movement */
	void updateCamera(const std::string &camera, const double &deltaTime);
    
    /* Update Player Camera */
    void updatePlayerCamera(const std::string &camera, PlayerPtr _player, const double &deltaTime);
    
    void checkCollision(const double &deltaTime, const double &elapsedTime);
    
    void updateGameVariables();
    void endGameAnimation(const double &deltaTime, const double &elapsedTime);
    void updateFogVariables(std::string shaderName);
    void updateSkyBoxVariables(vmml::Vector3f skyColor, float gradient, float density);
    void reloadGame();

	/* Helper Functions */
	GLfloat randomNumber(GLfloat min, GLfloat max);
    
	/* Variables */
	GLfloat _randomOffset;
	GLfloat _offset;
	GLfloat _cameraSpeed;
	double _mouseX, _mouseY;
	bool _running = false; 
	GLint _lastStateSpaceKey = 0;
	vmml::Matrix4f _viewMatrixHUD;
	double elapsedTime = 0.0;
    TerrainLoaderPtr _terrainLoader;
    PlayerPtr _player;
    CamPtr _cam;
    TerrainPtr _terrain;
    PlayerCameraPtr _playerCamera;
    ShadowModelRendererPtr _shadowModelRenderer;
    BloomRendererPtr _bloomRenderer;
	SunPtr _sun;
    SkydomePtr _skydome;
    std::vector<TerrainPtr> terrains;
	StartScreenRendererPtr _startScreenRenderer;
    
    float _fogDensity;
    float _fogGradient;
    vmml::Vector3f _fogColor;
    
    float _torchDamper = 1.0;
    float _torchInnerCutOff = cos(M_PI_F/180 * 15.0);
    float _torchOuterCutOff = cos(M_PI_F/180 * 20.0);

	int currentSecond;
    int _nrOfCollectedCrystals = 0;
    int _maxNrOfCollectedCrystals = 3;
    bool _gameHasEnded = false;
    
    bool _animation_forward;
    float _animation;
    float _animationSpeed;
    
    bool _loadNewGame = false;
};

#endif /* defined(PROJECT_MAIN_H) */
