#ifndef B_PLAYERCAMERA_H
#define B_PLAYERCAMERA_H

#include "bRenderer.h"
#include "Player.h"

class PlayerCamera
{
public:
    PlayerCamera();
    PlayerCamera(std::string cameraName, PlayerPtr player, Renderer &renderer);
    
    void move();
    vmml::Vector3f getPosition();
    float getPitch();
    float getYaw();
    float getRoll();
	float getAngle();
    
private:
    CameraPtr _camera;
    PlayerPtr _player;
    Renderer _renderer;
    
    vmml::Vector3f _position = vmml::Vector3f(0.0);
    
    void calculateZoom(); // calculate distance from player
    void calculateAngleAroundPlayer(); // angle around player
    void calculatePitch();
    
    float calculateHorizontalDistance(); // return distanceFromPlayer * cos(radians(pitch));
    float calculateVerticalDistance(); // return distanceFromPlayer * sin(radians(pitch));
    void calculateCameraPosition(float horizDistance, float verticDistance);
    
    float degreeToRadians(float degree);

    float _distanceFromPlayer = 100.0;
    float _angleAroundPlayer = 0.0;
	float _theta;
    
    bool _moved = false;
    
    float _pitch = -M_PI_F/6.0;
    float _yaw;
    float _roll;
    float _lazy = 0.90;
};
typedef std::shared_ptr< PlayerCamera >  PlayerCameraPtr;
#endif /* defined(B_PLAYERCAMERA_H) */
