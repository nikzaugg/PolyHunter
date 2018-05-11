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
    float horizontalDistance = calculateHorizontalDistance();
    float verticalDistance = calculateVerticalDistance();
    calculateCameraPosition(horizontalDistance, verticalDistance);
    vmml::Vector3f initialPosition = vmml::Vector3f(-_position.x(), _position.y(), -_position.z());
    std::cout << "initial position of camera: " <<initialPosition << std::endl;
    _yaw = 180.0 - (-1.0)*(_player->getRotY()+ 90.0 + _angleAroundPlayer);
    _yaw = degreeToRadians(_yaw);
    _camera->setPosition(initialPosition);
    _camera->setRotation(vmml::Vector3f(_pitch, M_PI_F/2.0 - _yaw, 0.f));
    
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
    vmml::Vector3f currentPosition = _camera->getPosition();
    float currentYaw = _yaw;
//    std::cout << "horzontalDistance: " << horizontalDistance << std::endl;
//    std::cout << "verticalDistance: " << verticalDistance << std::endl;
    calculateCameraPosition(horizontalDistance, verticalDistance);
    float targetYaw = 180.0 - (-1.0)*(_player->getRotY()+ 90.0 + _angleAroundPlayer);
    targetYaw = degreeToRadians(targetYaw);
//    std::cout << _yaw << std::endl;
//    std::cout << "PlayerPosition: " << _player->getPosition() << std::endl;
//    std::cout << "CameraPosition: " << _position.x() << " | " << _position.y() << " | " << _position.z() << std::endl;
    
    std::cout << "CurrentCameraPosition: " << _camera->getPosition() << std::endl;
    vmml::Vector3f nextPosition;
    vmml::Vector3f targetPosition = vmml::Vector3f(-_position.x(), _position.y(), -_position.z());
    
    float deltaX = -_position.x() - currentPosition.x();
    float deltaY = _position.y() - currentPosition.y();
    float deltaZ = -_position.z() - currentPosition.z();
    
    std::cout << "deltaX: "<< deltaX <<std::endl;
    std::cout << "deltaY: "<< deltaY <<std::endl;
    std::cout << "deltaZ: "<< deltaZ <<std::endl;
    
    // if moving
//        std::cout << "queue size: " << cameraQueue.size() << std::endl;
//     if (deltaY > 0.0 ||deltaY > 0.0 ||deltaZ > 0.0) {
//        for (int i = 2; i>0; i--) {
//            CameraFrame frame = CameraFrame();
//            frame.position = currentPosition;
//            frame.position[0] += deltaX/(float)i;
//            frame.position[1] += deltaY/(float)i;
//            frame.position[2] += deltaZ/(float)i;
//            std::cout << "FramePosition: " << frame.position << std::endl;
//            cameraQueue.push(frame);
//        }
//        CameraFrame nextFrame = cameraQueue.front();
//        std::cout << "nextFrame: " << nextFrame.position << std::endl;
//        nextPosition = vmml::Vector3f(nextFrame.position.x(), nextFrame.position.y(), nextFrame.position.z());
//        cameraQueue.pop();
//    }else { // not moving
//        nextPosition = vmml::Vector3f(-_position.x(), _position.y(), -_position.z());
//        if(cameraQueue.size() > 0){
//            CameraFrame nextFrame = cameraQueue.front();
//            std::cout << "nextFrame: " << nextFrame.position << std::endl;
//            nextPosition = vmml::Vector3f(nextFrame.position.x(), nextFrame.position.y(), nextFrame.position.z());
//            cameraQueue.pop();
//            cameraQueue.pop();
//        }
//
//    }
    float lazy = 0.95;
    _yaw = (lazy * currentYaw) + ((1.0-lazy) * targetYaw);
    nextPosition.x() = (lazy * currentPosition.x()) + ((1.0-lazy) * targetPosition.x());
    nextPosition.y() = (lazy * currentPosition.y()) + ((1.0-lazy) * targetPosition.y());
    nextPosition.z() = (lazy * currentPosition.z()) + ((1.0-lazy) * targetPosition.z());
    
    // nextPosition = vmml::Vector3f(-_position.x(), _position.y(), -_position.z());
    std::cout << "nextPosition: " << nextPosition << std::endl;
    _camera->setPosition(nextPosition);
    _camera->setRotation(vmml::Vector3f(_pitch, M_PI_F/2.0 - _yaw, 0.f));
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
