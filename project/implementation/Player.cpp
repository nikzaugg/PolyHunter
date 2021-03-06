#include "Player.h"
#include "Terrain.h"
#include "bRenderer.h"

void Player::process(std::string cameraName, const double &deltaTime)
{
    // check for inputs and move player accordingly
    checkInputs(cameraName);

    increaseRotation(0.0, (float)deltaTime * _currentTurnSpeed, 0.0);
    float distance = _currentSpeed * deltaTime;
    //std::cout << "getRotY: "<< getRotY() << std::endl;
    float dx = (float)(distance * sin(degreeToRadians(getRotY())));
    //std::cout << "dx: "<< dx << std::endl;
    float dz = (float)(distance * cos(degreeToRadians(getRotY())));
    //std::cout << "dz: "<< dz << std::endl;
    increasePosition(dx, 0.0, dz);
    //std::cout << "Position after: " << getPosition() << std::endl;

    _upwardsSpeed += GRAVITY * deltaTime;
    increasePosition(0.0, _upwardsSpeed * deltaTime, 0.0);
    float terrainHeight = Terrain::getHeightFromNoise(Terrain::getNoiseInput(getPosition().x()), Terrain::getNoiseInput(getPosition().z()));

    setYPosition(terrainHeight);
    render(cameraName);
}

void Player::customProcess(std::string cameraName, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    // check for inputs and move player accordingly
    checkInputs(cameraName);
    
    increaseRotation(0.0, (float)deltaTime * _currentTurnSpeed, 0.0);
    float distance = _currentSpeed * deltaTime;
    //std::cout << "getRotY: "<< getRotY() << std::endl;
    float dx = (float)(distance * sin(degreeToRadians(getRotY())));
    //std::cout << "dx: "<< dx << std::endl;
    float dz = (float)(distance * cos(degreeToRadians(getRotY())));
    //std::cout << "dz: "<< dz << std::endl;
    increasePosition(dx, 0.0, dz);
    //std::cout << "Position after: " << getPosition() << std::endl;
    
    _upwardsSpeed += GRAVITY * deltaTime;
    increasePosition(0.0, _upwardsSpeed * deltaTime, 0.0);
	float terrainHeight = Terrain::getHeightFromNoise(Terrain::getNoiseInput(getPosition().x()), Terrain::getNoiseInput(getPosition().z()));
    
    setYPosition(terrainHeight);
    customRender(cameraName, view, proj);
}

void Player::checkInputs(std::string cameraName) {

    //// Adjust aspect ratio ////
    renderer().getObjects()->getCamera(cameraName)->setAspectRatio(renderer().getView()->getAspectRatio());

    if (!Input::isTouchDevice()) {
        if (renderer().getInput()->getKeyState(bRenderer::KEY_W) == bRenderer::INPUT_PRESS) {
            _currentSpeed = -RUN_SPEED;
        }
        else if (renderer().getInput()->getKeyState(bRenderer::KEY_S) == bRenderer::INPUT_PRESS) {
            _currentSpeed = RUN_SPEED;
        }
        else {
            _currentSpeed = 0.0;
        }

        if (renderer().getInput()->getKeyState(bRenderer::KEY_A) == bRenderer::INPUT_PRESS) {
            _currentTurnSpeed = -TURN_SPEED;
        }
        else if (renderer().getInput()->getKeyState(bRenderer::KEY_D) == bRenderer::INPUT_PRESS) {
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
            // If touch is in lower half of screen
            if(touch.startPositionY > renderer().getView()->getHeight() / 2) {
                // If touch is in left portion of screen
                if (touch.startPositionX < renderer().getView()->getWidth() / 2) {
                    _currentSpeed = RUN_SPEED * (touch.currentPositionY - touch.startPositionY) / 100;
                    if(_currentSpeed > RUN_SPEED){
                        _currentSpeed = RUN_SPEED;
                    }
                    if(_currentSpeed < -RUN_SPEED){
                        _currentSpeed = -RUN_SPEED;
                    }
                // If touch is in right portion of screen
                } else {
                    _currentTurnSpeed = TURN_SPEED * (touch.currentPositionX - touch.startPositionX) / 300;
                }
        
            if (++i > 2)
                break;
            }
        }
    }
}

void Player::render(std::string camera)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    MaterialPtr playerMat = renderer().getObjects()->createMaterial("playerMaterial", renderer().getObjects()->getShader("player"));
    
    // draw model
    renderer().getModelRenderer()->queueModelInstance(getModelName(), "player",camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}

void Player::customRender(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // renderer().getObjects()->getModel("player")->setMaterial(playerShadowMat);

    // draw model
    renderer().getModelRenderer()->drawModel(renderer().getObjects()->getModel(getModelName()), computeTransformationMatrix(), view, proj, std::vector<std::string>({}), false);
    
    

}

void Player::test() {
    std::cout << "<------- Player called ---------->" << std::endl;
}
