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
    
private:
    CameraPtr _camera;
    PlayerPtr _player;
    Renderer _renderer;
    
    vmml::Vector3f _position = vmml::Vector3f(0.0);
    
    void calculateZoom(); // calculate distance from player
    void calculateAngleAroundPlayer(); // angle around player
    
    float calculateHorizontalDistance(); // return distanceFromPlayer * cos(radians(pitch));
    float calculateVerticalDistance(); // return distanceFromPlayer * sin(radians(pitch));
    void calculateCameraPosition(float horizDistance, float verticDistance);
    
    float degreeToRadians(float degree);
    
    float _distanceFromPlayer = 100.0;
    float _angleAroundPlayer = 0.0;
    
    float _pitch = M_PI_F/2.0;
    float _yaw;
    float _roll;
};
typedef std::shared_ptr< PlayerCamera >  PlayerCameraPtr;
#endif /* defined(B_PLAYERCAMERA_H) */
