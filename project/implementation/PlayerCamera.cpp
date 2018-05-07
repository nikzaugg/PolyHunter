#include "bRenderer.h"
#include "PlayerCamera.h"


void PlayerCamera::move(){
    calculateZoom();
    calculateAngleAroundPlayer();
    float horizontalDistance = calculateHorizontalDistance();
    float verticalDistance = calculateVerticalDistance();
    calculateCamerPosition(horizontalDistance, verticalDistance);
    yaw = 180.0 - (player->getRotY() + angleAroundPlayer);
}

void PlayerCamera::calculateCamerPosition(float horizDistance, float verticDistance){
    float theta = player->getRotY() + angleAroundPlayer;
    float offsetX = (float)horizDistance * sin(theta);
    float offsetZ = (float)horizDistance * cos(theta);
    position.x() = player->getPosition().x() - offsetX;
    position.z() = player->getPosition().z() - offsetZ;
    position.y() = player->getPosition().y() + verticDistance;
}
