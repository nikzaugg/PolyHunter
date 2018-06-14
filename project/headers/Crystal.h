#ifndef B_CRYSTAL_H
#define B_CRYSTAL_H

#include "bRenderer.h"
#include "Entity.h"

class Crystal : public Entity
{
    
public:
    Crystal() : Entity() {};
    Crystal(std::string crystalName, std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale) : Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
    {
        _crystalName = crystalName;
    };
    
    void drawNormalsOnly(std::string camera, const double &deltaTime, std::string entityName);
    void draw(std::string camera);
    void render(std::string camera);
    void customRender(std::string camera, vmml::Matrix4f view ,vmml::Matrix4f proj);
    
private:
    std::string _crystalName;
    
};
typedef std::shared_ptr< Crystal >  CrystalPtr;
#endif /* defined(B_CRYSTAL_H) */

