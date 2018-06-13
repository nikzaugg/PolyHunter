#ifndef B_PLAYER_H
#define B_PLAYER_H

#include "bRenderer.h"
#include "Entity.h"
#include "Terrain.h"

class Player : public Entity
{
public:
    Player() : Entity() {};
    Player(std::string objName,std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
    : Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale) {};
    Player(Renderer& renderer);
    
    void render(std::string camera);
    void customRender(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj);
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
    void test();
    void process(std::string cameraName, const double &deltaTime);
    void customProcess(std::string cameraName, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj);
    void checkInputs(std::string cameraName);
    
private:
    int _numFaces;
    
    const float RUN_SPEED = 20.0;
    const float TURN_SPEED = 60.0;
    float _currentSpeed = 0.0;
    float _currentTurnSpeed = 0.0;
    
    const float GRAVITY = -50.0;
    const float JUMP_POWER = 30;
    float _upwardsSpeed = 0.0;
    
    const float TERRAIN_HEIGHT = 1.0;
    
    bool _isInAir = false;
    
    
    GroupMap        _groups;
    vmml::AABBf        _boundingBox;
};
typedef std::shared_ptr< Player >  PlayerPtr;
#endif /* defined(B_PLAYER_H) */
