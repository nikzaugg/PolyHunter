#include "bRenderer.h"
#include "PlayerCamera.h"

PlayerCamera::PlayerCamera(std::string cameraName, PlayerPtr player, Renderer &renderer)
{
    _player = player;
    _renderer = renderer;
    _camera = renderer.getObjects()->getCamera(cameraName);
    
//    _camera->setPosition(vmml::Vector3f(-25.0, -_player->getPosition().y()-100.0, -25.0));
//    _camera->setRotation(vmml::Vector3f(-M_PI_F/2.0, -M_PI_F/2.0 ,0.0));
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
//    std::cout << "horzontalDistance: " << horizontalDistance << std::endl;
//    std::cout << "verticalDistance: " << verticalDistance << std::endl;
    calculateCameraPosition(horizontalDistance, verticalDistance);
    _yaw = 180.0 - (-1.0)*(_player->getRotY()+90.0 + _angleAroundPlayer);
    _yaw = degreeToRadians(_yaw);
//    std::cout << _yaw << std::endl;
//    std::cout << "PlayerPosition: " << _player->getPosition() << std::endl;
//    std::cout << "CameraPosition: " << _position.x() << " | " << _position.y() << " | " << _position.z() << std::endl;
    _camera->setPosition(vmml::Vector3f(-_position.x(), _position.y(), -_position.z()));
    _camera->setRotation(vmml::Vector3f(-M_PI_F/4.0, M_PI_F/2.0 - _yaw, 0.f));
}

void PlayerCamera::calculateCameraPosition(float horizDistance, float verticDistance){
//    std::cout << "horiz: " << horizDistance << " | " << "vertical: " << verticDistance << std::endl;
    float theta = (-1.0)*(_player->getRotY() + 90.0 + _angleAroundPlayer);
//    std::cout << "theta in radians: "<< degreeToRadians(theta) << std::endl;
    float offsetX = (float)horizDistance * cos(degreeToRadians(theta));
    float offsetZ = (float)horizDistance * sin(degreeToRadians(theta));
    _position.x() = _player->getPosition().x() - offsetX;
    _position.y() = (-1.0)*(_player->getPosition().y() + verticDistance);
    _position.z() = _player->getPosition().z() - offsetZ;
//    std::cout << "offsetX: " << offsetX << " | " << "offsetZ: " << offsetZ << std::endl;
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
