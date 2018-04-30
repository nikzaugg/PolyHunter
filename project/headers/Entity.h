#ifndef B_ENTITY_H
#define B_ENTITY_H

#include "bRenderer.h"

class Entity
{
public:
    Entity();
    Entity(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
    
    Entity(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
    
    void increasePosition(float dx, float dy, float dz);
    void increaseRotation(float dx, float dy, float dz);
    void SetModel(ModelPtr model);
    ModelPtr getModel();
    void setModelName(std::string modelName);
    std::string getModelName();
    ShaderPtr getShader();
    MaterialPtr getMaterial();
    PropertiesPtr getProperties();
    std::string getMaterialName();
    vmml::Vector3f getPosition();
    void setPosition(vmml::Vector3f);
    void setYPosition(float y);
    void setRotX(float rotX);
    float getRotX();
    void setRotY(float rotY);
    float getRotY();
    void setRotZ(float rotZ);
    float getRotZ();
    void setScale(float scale);
    float getScale();
    Renderer renderer() { return _renderer; };
    vmml::Matrix4f computeTransformationMatrix();
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
    
private:
    vmml::Matrix4f _modelMatrix;
    MaterialPtr _material;
    PropertiesPtr _properties;
    ShaderPtr _shader;
    ModelPtr _model;
    Renderer _renderer;
    
    std::string _modelName;
    std::string _propertiesName;
    std::string _shaderName;
    std::string _materialName;
    std::string _materialFile;
    
    vmml::Vector3f _position;
    float _rotX;
    float _rotY;
    float _rotZ;
    float _scale;
    
    GroupMap        _groups;
    vmml::AABBf        _boundingBox;
};
#endif /* defined(B_ENTITY_H) */
