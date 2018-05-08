#include "Player.h"
#include "Terrain.h"
#include "bRenderer.h"

void Player::process(std::string cameraName, const double &deltaTime)
{
    // check for inputs and move player accordingly
    checkInputs();

    increaseRotation(0.0, (float)deltaTime * _currentTurnSpeed, 0.0);
    float distance = _currentSpeed * deltaTime;
    //std::cout << "distance: "<< distance << std::endl;
    float dx = (float)(distance * cos(getRotY()));
    //std::cout << "dx: "<< dx << std::endl;
    float dz = (float)(distance * sin(getRotY()));
    //std::cout << "dz: "<< dz << std::endl;
    increasePosition(dx, 0.0, dz);
    //std::cout << "Position after: " << getPosition() << std::endl;

    _upwardsSpeed += GRAVITY * deltaTime;
    increasePosition(0.0, _upwardsSpeed * deltaTime, 0.0);
    float terrainHeight = getHeightFromNoise(getNoiseInput(getPosition().x()), getNoiseInput(getPosition().z()));
    setYPosition(terrainHeight);
    render(cameraName);
}

double Player::getNoiseInput(float coord)
{
    // FIXME: instead of 100.0, add _TERRAIN_SIZE
    return coord / (float)150.0;
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

void Player::test() {
    std::cout << "<------- Player called ---------->" << std::endl;
}
