#include "Entity.h"
#include "bRenderer.h"

Entity::Entity()
{
}

Entity::Entity(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
{
    _renderer = renderer;
    _modelName = modelName;
    _propertiesName = propName;
    _shader = shader;
    this->_properties = _renderer.getObjects()->createProperties(_propertiesName);
    this->_model = _renderer.getObjects()->loadObjModel(objName, false, true, _shader, _properties);
    this->_position = pos;
    this->_rotX = rotX;
    this->_rotY = rotY;
    this->_rotZ = rotZ; 
    this->_scale = scale;
}

void Entity::increasePosition(float dx, float dy, float dz)
{
    _position.x() += dx;
    _position.y() += dy;
    _position.z() += dz;
}

void Entity::increaseRotation(float dx, float dy, float dz)
{
    _rotX += dx;
    _rotY += dy;
    _rotZ += dz;
}

ModelPtr Entity::getModel()
{
    return _model;
}

void Entity::setModelName(std::string modelName) {
    _modelName = modelName;
}

std::string Entity::getModelName() {
    return _modelName;
}

void Entity::SetModel(ModelPtr model)
{
    _model = model;
}

vmml::Vector3f Entity::getPosition()
{
    return _position;
}

void Entity::setPosition(vmml::Vector3f pos)
{
    _position = pos;
}

void Entity::setYPosition(float y)
{
    _position[1] = y;
}

void Entity::setRotX(float rotX)
{
    _rotX = rotX;
}

float Entity::getRotX()
{
    return _rotX;
}

void Entity::setRotY(float rotY)
{
    _rotY = rotY;
}

float Entity::getRotY()
{
    return _rotY;
}

void Entity::setRotZ(float rotZ)
{
    _rotZ = rotZ;
}

float Entity::getRotZ()
{
    return _rotZ;
}

void Entity::setScale(float scale)
{
    _scale = scale;
}

float Entity::getScale()
{
    return _scale;
}

vmml::Matrix4f Entity::computeTransformationMatrix()
{
    vmml::Matrix4f modelMatrix;
    modelMatrix *= vmml::create_translation(vmml::Vector3f(_position[0], _position[1], _position[2]));
    modelMatrix *= vmml::create_rotation(_rotX, vmml::Vector3f::UNIT_X);
    modelMatrix *= vmml::create_rotation(_rotY, vmml::Vector3f::UNIT_Y);
    modelMatrix *= vmml::create_rotation(_rotZ, vmml::Vector3f::UNIT_Z);
    modelMatrix *= vmml::create_scaling(vmml::Vector3f(_scale));
    return modelMatrix;
}
