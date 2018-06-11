#include "bRenderer.h"
#include "Cam.h"
#include "noise.h"


Cam::Cam(Renderer &renderer)
{
    _renderer = renderer;
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

		// use space to pause and unpause
		GLint currentStateSpaceKey = renderer().getInput()->getKeyState(bRenderer::KEY_SPACE);
		if (currentStateSpaceKey != _lastStateSpaceKey)
		{
			_lastStateSpaceKey = currentStateSpaceKey;
			if (currentStateSpaceKey == bRenderer::INPUT_PRESS) {
				_running = !_running;
				renderer().getInput()->setCursorEnabled(!_running);
			}
		}

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
		{
			if (renderer().getInput()->getKeyState(bRenderer::KEY_LEFT_SHIFT) == bRenderer::INPUT_PRESS)
				cameraForward = 2.0;
			else
				cameraForward = 1.0;
		}
		else if (renderer().getInput()->getKeyState(bRenderer::KEY_S) == bRenderer::INPUT_PRESS)
		{
			if (renderer().getInput()->getKeyState(bRenderer::KEY_LEFT_SHIFT) == bRenderer::INPUT_PRESS)
				cameraForward = -2.0;
			else
				cameraForward = -1.0;
		}
		else {
			cameraForward = 0.0;
		}
			
            
            
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
    float currentX = currentPosition.x();
    float currentZ = currentPosition.z();
    float height = (-1.0) * getHeightFromNoise(getNoiseInput(-currentX), getNoiseInput(-currentZ));
    height -= _cameraFloorOffset;
    _position = vmml::Vector3f(currentX, height, currentZ);
    //std::cout << "camposition: "<< _position << std::endl;
    renderer().getObjects()->getCamera(camera)->setPosition(vmml::Vector3f(_position));
    
}


double Cam::getNoiseInput(float coord)
{
    // FIXME: add variable instead of magic number
    return coord / (float)(400.0 * 3);
}

double Cam::noise(double nx, double nz)
{
    noise::module::Perlin perlin;
    perlin.SetSeed(549);
    perlin.SetOctaveCount(4);
    perlin.SetFrequency(1);
    perlin.SetLacunarity(2.5);
    
    // Rescale from -1.0:+1.0 to 0.0:1.0
    double value = perlin.GetValue(nx, nz, 0.0) / 2.0 + 0.5;
    
    // Prevent NaN error by not allowing values below 0
    value = value < 0.0 ? 0.0 : value;
    
    return value;
}

float Cam::getHeightFromNoise(double nx, double nz)
{
    // Rescale from -1.0:+1.0 to 0.0:1.0
    float res = noise(nx, nz);
    res = pow(res, 1.27);
    res *= 128;
    return res;
}

vmml::Vector3f Cam::getPosition()
{
    // return _position;
    return vmml::Vector3f(-_position.x(), -_position.y(), -_position.z());
}

float Cam::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}
