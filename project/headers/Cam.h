#ifndef B_Cam_H
#define B_Cam_H

#include "bRenderer.h"
#include "Player.h"

class Cam
{
public:
    Cam();
    Cam(Renderer &renderer);
    void process(std::string camera, const double &deltaTime);
    vmml::Vector3f getPosition();
	void setMovable(bool enable);
	void setPosition(vmml::Vector3f position);
    
private:
    CameraPtr _camera;
    Renderer _renderer;
	vmml::Matrix4f _viewMatrixHUD;
    
    Renderer renderer() {
        return _renderer;
    }
    
    vmml::Vector3f _position = vmml::Vector3f(0.0);
    float _cameraFloorOffset = 10.0;
    float _dx;
    float _dz;
    GLfloat _cameraSpeed = 40.0;
    double _mouseX, _mouseY;
    GLint _lastStateSpaceKey = 0;
	bool _running = true;
	bool _movementEnabled;
    // Helper Functions
    float degreeToRadians(float degree);
};
typedef std::shared_ptr< Cam >  CamPtr;
#endif /* defined(B_Cam_H) */
