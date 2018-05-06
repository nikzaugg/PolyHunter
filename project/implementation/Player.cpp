#include "Player.h"
#include "Terrain.h"
#include "bRenderer.h"

void Player::test() {
    std::cout << "<------- Player called ---------->" << std::endl;
}

void Player::process(std::string cameraName, const double &deltaTime, TerrainPtr terrain)
{
    // check for inputs and move player accordingly
    checkInputs();
    
    increaseRotation(0.0, (float)deltaTime * _currentTurnSpeed, 0.0);
    float distance = _currentSpeed * deltaTime;
    float dx = (float)(distance * sin(getRotY()));
    float dz = (float)(distance * cos(getRotY()));
    for (int i = 0; i < 10; i++) {
        increasePosition(dx/10.0, 0.0, dz/10.0);
    }
    
    // std::cout << getPosition() << std::endl;

    _upwardsSpeed += GRAVITY * deltaTime;
    increasePosition(0.0, _upwardsSpeed * deltaTime, 0.0);
    float terrainHeight = terrain->getHeightOfTerrain(getPosition().z(), getPosition().x());
    // std::cout << terrainHeight << std::endl;
    if (getPosition().y() < terrainHeight)
    {
        _upwardsSpeed = 0.0;
        _isInAir = false;
        setYPosition(terrainHeight);
    }
    // draw
    render(cameraName);
}


float Player::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}

void Player::checkInputs() {
    
    //// Adjust aspect ratio ////
    renderer().getObjects()->getCamera("camera")->setAspectRatio(renderer().getView()->getAspectRatio());
    
    if (!Input::isTouchDevice()) {
        if (renderer().getInput()->getKeyState(bRenderer::KEY_I) == bRenderer::INPUT_PRESS) {
            _currentSpeed = RUN_SPEED;
        }
        else if (renderer().getInput()->getKeyState(bRenderer::KEY_K) == bRenderer::INPUT_PRESS) {
            _currentSpeed = -RUN_SPEED;
        }
        else {
            _currentSpeed = 2.0;
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
    } else {
        // control using touch
        TouchMap touchMap = renderer().getInput()->getTouches();
        int i = 0;
        for (auto t = touchMap.begin(); t != touchMap.end(); ++t)
        {
            Touch touch = t->second;
            // If touch is in left half of the view: move around
            if (touch.startPositionX < renderer().getView()->getWidth() / 2){
                _currentSpeed = RUN_SPEED * -(touch.currentPositionY - touch.startPositionY) / 100;
                if(_currentSpeed > RUN_SPEED){
                    _currentSpeed = RUN_SPEED;
                }
                if(_currentSpeed < -RUN_SPEED){
                    _currentSpeed = -RUN_SPEED;
                }
            }
            // if touch is in right half of the view: look around
            else
            {
                _currentTurnSpeed = 0.0;
            }
            
            if (touch.startPositionX > renderer().getView()->getWidth() / 2) {
                _currentTurnSpeed = TURN_SPEED * (touch.currentPositionX - touch.startPositionX) / 500;
            } else {
                _currentTurnSpeed = 0.0;
            }
            if (++i > 2)
                break;
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
