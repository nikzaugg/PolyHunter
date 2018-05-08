#include "bRenderer.h"
#include "PlayerCamera.h"

PlayerCamera::PlayerCamera(std::string cameraName, PlayerPtr player, Renderer &renderer)
{
    _player = player;
    _renderer = renderer;
    _camera = renderer.getObjects()->getCamera(cameraName);
}

float PlayerCamera::getPitch()
{
    return _pitch;
}

void PlayerCamera::move(){
    calculateZoom();
    calculateAngleAroundPlayer();
    float horizontalDistance = calculateHorizontalDistance();
    float verticalDistance = calculateVerticalDistance();
    calculateCameraPosition(horizontalDistance, verticalDistance);
    _yaw = 180.0 - (_player->getRotY() + _angleAroundPlayer);
    _yaw = degreeToRadians(_yaw);
    std::cout << _yaw << std::endl;
    
//    _camera->setPosition(vmml::Vector3f(_position.x(), _position.y(), _position.z()));
//    _camera->setRotation(vmml::Vector3f(_pitch, -_yaw, 0.0));
    
    _camera->setPosition(vmml::Vector3f(-_player->getPosition().x(), (-1.0)*(_player->getPosition().y() + 50.0), -_player->getPosition().z() + 30.0));
    _camera->setRotation(vmml::Vector3f(-_pitch/2.0, -_yaw, 0.0));
}


void PlayerCamera::calculateCameraPosition(float horizDistance, float verticDistance){
    std::cout << "horiz: " << horizDistance << " | " << "vertical: " << verticDistance << std::endl;
    float theta = _player->getRotY() + _angleAroundPlayer;
    float offsetX = (float)horizDistance * cos(theta);
    float offsetZ = (float)horizDistance * sin(theta);
    _position.x() = _player->getPosition().x() - offsetX;
    _position.y() = _player->getPosition().y() + verticDistance;
    _position.z() = _player->getPosition().z() - offsetZ;
    std::cout << "posX: " << _position.x() << std::endl;
    std::cout << "offsetX: " << offsetX << " | " << "offsetZ: " << offsetZ << std::endl;
}

void PlayerCamera::calculateZoom()
{
    
}

float PlayerCamera::calculateVerticalDistance()
{
    return (float)_distanceFromPlayer * sin(_pitch);
}

float PlayerCamera::calculateHorizontalDistance()
{
    return (float)_distanceFromPlayer * cos(_pitch);
}

void PlayerCamera::calculateAngleAroundPlayer()
{
    _angleAroundPlayer =  _player->getRotY();
}

float PlayerCamera::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}
