#include "bRenderer.h"
#include "Cam.h"
#include "noise.h"


Cam::Cam(std::string cameraName,  Renderer &renderer)
{
    //_position = vmml::Vector3f(0.0);
    _renderer = renderer;
    // _renderer.getObjects()->getCamera(cameraName)->setPosition(_position);
}

void Cam::process(std::string camera, const double &deltaTime)
{
    //// Adjust aspect ratio ////
    renderer().getObjects()->getCamera(camera)->setAspectRatio(renderer().getView()->getAspectRatio());
    
    double deltaCameraY = 0.0;
    double deltaCameraX = 0.0;
    double cameraForward = 0.0;
    double cameraSideward = 0.0;
    
    /* iOS: control movement using touch screen */
    if (Input::isTouchDevice()){
            // control using touch
            TouchMap touchMap = renderer().getInput()->getTouches();
            int i = 0;
            for (auto t = touchMap.begin(); t != touchMap.end(); ++t)
            {
                Touch touch = t->second;
                // If touch is in left half of the view: move around
                if (touch.startPositionX < renderer().getView()->getWidth() / 2){
                    cameraForward = -(touch.currentPositionY - touch.startPositionY) / 100;
                    cameraSideward = (touch.currentPositionX - touch.startPositionX) / 100;
                    
                }
                // if touch is in right half of the view: look around
                else
                {
                    deltaCameraY = (touch.currentPositionX - touch.startPositionX) / 2000;
                    deltaCameraX = (touch.currentPositionY - touch.startPositionY) / 2000;
                }
                if (++i > 2)
                    break;
            }
    }
    /* Windows: control movement using mouse and keyboard */
    else {
        // mouse look
        double xpos, ypos;
        bool hasCursor = false;
        
        renderer().getInput()->getCursorPosition(&xpos, &ypos, &hasCursor);
        
        deltaCameraY = (xpos - _mouseX) / 1000;
        _mouseX = xpos;
        deltaCameraX = (ypos - _mouseY) / 1000;
        _mouseY = ypos;
        

            // movement using wasd keys
            if (renderer().getInput()->getKeyState(bRenderer::KEY_W) == bRenderer::INPUT_PRESS)
                if (renderer().getInput()->getKeyState(bRenderer::KEY_LEFT_SHIFT) == bRenderer::INPUT_PRESS)
                    cameraForward = 1.0;
                else
                    cameraForward = -1.0;
                else if (renderer().getInput()->getKeyState(bRenderer::KEY_S) == bRenderer::INPUT_PRESS)
                    if (renderer().getInput()->getKeyState(bRenderer::KEY_LEFT_SHIFT) == bRenderer::INPUT_PRESS)
                        cameraForward = -2.0;
                    else
                        cameraForward = -1.0;
                    else
                        cameraForward = 0.0;
            
            if (renderer().getInput()->getKeyState(bRenderer::KEY_A) == bRenderer::INPUT_PRESS)
                cameraSideward = -1.0;
            else if (renderer().getInput()->getKeyState(bRenderer::KEY_D) == bRenderer::INPUT_PRESS)
                cameraSideward = 1.0;
            if (renderer().getInput()->getKeyState(bRenderer::KEY_UP) == bRenderer::INPUT_PRESS)
                renderer().getObjects()->getCamera(camera)->moveCameraUpward(_cameraSpeed*deltaTime);
            else if (renderer().getInput()->getKeyState(bRenderer::KEY_DOWN) == bRenderer::INPUT_PRESS)
                renderer().getObjects()->getCamera(camera)->moveCameraUpward(-_cameraSpeed*deltaTime);
            if (renderer().getInput()->getKeyState(bRenderer::KEY_LEFT) == bRenderer::INPUT_PRESS)
                renderer().getObjects()->getCamera(camera)->rotateCamera(0.0f, 0.0f, 0.03f*_cameraSpeed*deltaTime);
            else if (renderer().getInput()->getKeyState(bRenderer::KEY_RIGHT) == bRenderer::INPUT_PRESS)
                renderer().getObjects()->getCamera(camera)->rotateCamera(0.0f, 0.0f, -0.03f*_cameraSpeed*deltaTime);
    }
    
    _dx = cameraForward*_cameraSpeed*deltaTime;
    _dz = cameraSideward*_cameraSpeed*deltaTime;
    
    //// Update camera ////
    renderer().getObjects()->getCamera(camera)->moveCameraForward(_dx);
    renderer().getObjects()->getCamera(camera)->rotateCamera(deltaCameraX, deltaCameraY, 0.0f);
    renderer().getObjects()->getCamera(camera)->moveCameraSideward(_dz);
    
    vmml::Vector3f currentPosition = renderer().getObjects()->getCamera(camera)->getPosition();
    float height = getHeightFromNoise(getNoiseInput(currentPosition.x()), getNoiseInput(currentPosition.z()));
    renderer().getObjects()->getCamera(camera)->setPosition(vmml::Vector3f(currentPosition.x(), height, currentPosition.z()));
    _position = vmml::Vector3f(currentPosition.x(), height, currentPosition.z());
}

void Cam::checkInputs(std::string cameraName) {
    
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

double Cam::getNoiseInput(float coord)
{
    // FIXME: instead of 100.0, add _TERRAIN_SIZE
    return coord / (float)450;
}

double Cam::noise(double nx, double nz)
{
    noise::module::Perlin perlin;
    perlin.SetSeed(549);
    perlin.SetOctaveCount(3);
    perlin.SetFrequency(4);
    return perlin.GetValue(nx, nz, 0.0) / 2.0 + 0.5;
}

float Cam::getHeightFromNoise(double nx, double nz)
{
    // Rescale from -1.0:+1.0 to 0.0:1.0
    float res = (float)noise(nx, nz);
    
    res = pow(res, 3);
    res *= 70;
    return res;
}

void Cam::calculateCameraPosition(){

}

vmml::Vector3f Cam::getPosition()
{
    return _position;
}

float Cam::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}
