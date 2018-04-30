#include "Player.h"
#include "bRenderer.h"

void Player::test() {
    std::cout << "<------- Player called ---------->" << std::endl;
}

void Player::process(std::string cameraName, const double &deltaTime)
{
    // check for inputs
    checkInputs();
    
    increaseRotation(0.0, (float)deltaTime * _currentTurnSpeed, 0.0);
    float distance = _currentSpeed * deltaTime;
    float dx = (float)(distance * sin(getRotY()));
    float dz = (float)(distance * cos(getRotY()));
    increasePosition(dx, 0.0, dz);
    
    _upwardsSpeed += GRAVITY * deltaTime;
    increasePosition(0.0, _upwardsSpeed * deltaTime, 0.0);
    if (getPosition().y() < TERRAIN_HEIGHT)
    {
        _upwardsSpeed = 0.0;
        _isInAir = false;
        setYPosition(TERRAIN_HEIGHT);
    }
    // draw
    render(cameraName);
}


float Player::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}

void Player::checkInputs() {
    if (renderer().getInput()->getKeyState(bRenderer::KEY_I) == bRenderer::INPUT_PRESS) {
        _currentSpeed = RUN_SPEED;
    }
    else if (renderer().getInput()->getKeyState(bRenderer::KEY_K) == bRenderer::INPUT_PRESS) {
        _currentSpeed = -RUN_SPEED;
    }
    else {
        _currentSpeed = 0.0;
    }
    
    if (renderer().getInput()->getKeyState(bRenderer::KEY_J) == bRenderer::INPUT_PRESS) {
        _currentTurnSpeed = -TURN_SPEED;
    }
    else if (renderer().getInput()->getKeyState(bRenderer::KEY_L) == bRenderer::INPUT_PRESS) {
        _currentTurnSpeed = TURN_SPEED;
    }
    else {
        _currentTurnSpeed = 0.0;
    }
    
    if (renderer().getInput()->getKeyState(bRenderer::KEY_G) == bRenderer::INPUT_PRESS) {
        if (!_isInAir)
        {
            _upwardsSpeed = JUMP_POWER;
            _isInAir = true;
        }
        
    }
}

void Player::render(std::string camera)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}
