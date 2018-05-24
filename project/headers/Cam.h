#ifndef B_Cam_H
#define B_Cam_H

#include "bRenderer.h"
#include "Player.h"

class Cam
{
public:
    Cam();
    Cam(std::string cameraName, Renderer &renderer);
    
    void process(std::string camera, const double &deltaTime);
    void move();
    
    vmml::Vector3f getPosition();
    float getPitch();
    float getYaw();
    float getRoll();
    
private:
    CameraPtr _camera;
    Renderer _renderer;
    
    Renderer renderer() {
        return _renderer;
    }
    
    void checkInputs(std::string cameraName);
    void calculateCameraPosition();
    double getNoiseInput(float coord);
    float getHeightFromNoise(double nx, double nz);
    double noise(double nx, double nz);

    vmml::Vector3f _position = vmml::Vector3f(0.0);
    float _pitch = -M_PI_F/6.0;
    float _yaw;
    float _roll;
    float _lazy = 0.80;
    
    const float RUN_SPEED = 20.0;
    const float TURN_SPEED = 60.0;
    float _currentSpeed = 0.0;
    float _currentTurnSpeed = 0.0;
    
    float _dx;
    float _dz;
    float _yRot;
    float _xRot;
    GLfloat _cameraSpeed = 40.0;
    double _mouseX, _mouseY;
    GLint _lastStateSpaceKey = 0;
    
    // Helper Functions
    float degreeToRadians(float degree);
};
typedef std::shared_ptr< Cam >  CamPtr;
#endif /* defined(B_Cam_H) */
