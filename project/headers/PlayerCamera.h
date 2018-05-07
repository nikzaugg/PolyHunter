#ifndef B_PLAYERCAMERA_H
#define B_PLAYERCAMERA_H

#include "bRenderer.h"
#include "Player.h"

class PlayerCamera
{
public:
    PlayerCamera();
    PlayerCamera(std::string cameraName, PlayerPtr player);
    
    void move();
    vmml::Vector3f getPosition();
    float getPitch();
    float getYaw();
    float getRoll();
    
private:
    CameraPtr camera;
    PlayerPtr player;
    
    vmml::Vector3f position = vmml::Vector3f(0.0);
    
    void calculateZoom(); // calculate distance from player
    void calculateAngleAroundPlayer(); // angle around player
    
    float calculateHorizontalDistance(); // return distanceFromPlayer * cos(radians(pitch));
    float calculateVerticalDistance(); // return distanceFromPlayer * sin(radians(pitch));
    void calculateCamerPosition(float horizDistance, float verticDistance);
    
    float distanceFromPlayer = 100.0;
    float angleAroundPlayer = 0.0;
    
    float pitch = M_PI_F/4;
    float yaw;
    float roll;
};
typedef std::shared_ptr< PlayerCamera >  PlayerCameraPtr;
#endif /* defined(B_PLAYERCAMERA_H) */
