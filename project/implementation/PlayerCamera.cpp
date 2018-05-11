#include "bRenderer.h"
#include "PlayerCamera.h"
#include <queue>

struct CameraFrame
{
    vmml::Vector3f position;
};

std::queue<CameraFrame> cameraQueue;

PlayerCamera::PlayerCamera(std::string cameraName, PlayerPtr player, Renderer &renderer)
{
    _player = player;
    _renderer = renderer;
    _camera = renderer.getObjects()->getCamera(cameraName);
    
    // initialize the camera to be behind the player when the camera is created
    float horizontalDistance = calculateHorizontalDistance();
    float verticalDistance = calculateVerticalDistance();
    calculateCameraPosition(horizontalDistance, verticalDistance);
    vmml::Vector3f initialPosition = vmml::Vector3f(-_position.x(), _position.y(), -_position.z());
    std::cout << "initial position of camera: " <<initialPosition << std::endl;
    _yaw = 180.0 - (-1.0)*(_player->getRotY()+ 90.0 + _angleAroundPlayer);
    _yaw = degreeToRadians(_yaw);
    _camera->setPosition(initialPosition);
    _camera->setRotation(vmml::Vector3f(_pitch, M_PI_F/2.0 - _yaw, 0.f));
}

float PlayerCamera::getPitch()
{
    return _pitch;
}

void PlayerCamera::move(){
    calculateZoom();
    calculateAngleAroundPlayer();
    
    // calculate distances between player and camera
    float horizontalDistance = calculateHorizontalDistance();
    float verticalDistance = calculateVerticalDistance();
    
    // save position and yaw before calculating new position of camera
    vmml::Vector3f currentPosition = _camera->getPosition();
    float currentYaw = _yaw;
    
    // calculate new camera position (target position)
    calculateCameraPosition(horizontalDistance, verticalDistance);
    
    // calculate target yaw
    float targetYaw = 180.0 - (-1.0)*(_player->getRotY()+ 90.0 + _angleAroundPlayer);
    targetYaw = degreeToRadians(targetYaw);

    // asymptotic averaging of current position and target position with a lazy-factor
    vmml::Vector3f nextPosition;
    vmml::Vector3f targetPosition = vmml::Vector3f(-_position.x(), _position.y(), -_position.z());
    nextPosition.x() = (_lazy * currentPosition.x()) + ((1.0-_lazy) * targetPosition.x());
    nextPosition.y() = (_lazy * currentPosition.y()) + ((1.0-_lazy) * targetPosition.y());
    nextPosition.z() = (_lazy * currentPosition.z()) + ((1.0-_lazy) * targetPosition.z());
    
    // asymptotic averaging of current yaw and target yaw with a lazy-factor
    _yaw = (_lazy * currentYaw) + ((1.0-_lazy) * targetYaw);
    
    // set camera's position & it's yaw
    _camera->setPosition(nextPosition);
    _camera->setRotation(vmml::Vector3f(_pitch, M_PI_F/2.0 - _yaw, 0.f));
}

void PlayerCamera::calculateCameraPosition(float horizDistance, float verticDistance){
    // std::cout << "horiz: " << horizDistance << " | " << "vertical: " << verticDistance << std::endl;
    
    // Calculate the angle the player has on the Y-Axis (+90.0 as player model was loaded with a angle to face the correct direction)
    float theta = (-1.0)*(_player->getRotY() + 90.0 + _angleAroundPlayer);
    // std::cout << "theta in radians: "<< degreeToRadians(theta) << std::endl;
    
    // Calculate the offset in X and Z direction of the camera using basic trigonometry
    float offsetX = (float)horizDistance * cos(degreeToRadians(theta));
    float offsetZ = (float)horizDistance * sin(degreeToRadians(theta));
    _position.x() = _player->getPosition().x() - offsetX;
    _position.y() = (-1.0)*(_player->getPosition().y() + verticDistance);
    _position.z() = _player->getPosition().z() - offsetZ;
    // std::cout << "offsetX: " << offsetX << " | " << "offsetZ: " << offsetZ << std::endl;
}

void PlayerCamera::calculateZoom()
{
    
}

float PlayerCamera::calculateVerticalDistance()
{
    return abs((float)_distanceFromPlayer * sin(_pitch));
}

float PlayerCamera::calculateHorizontalDistance()
{
    return abs((float)_distanceFromPlayer * cos(_pitch));
}

void PlayerCamera::calculateAngleAroundPlayer()
{
    _angleAroundPlayer =  0.0;
}

float PlayerCamera::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}
