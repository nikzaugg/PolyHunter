#include "Entity.h"
#include "bRenderer.h"
#include "noise.h"


Entity::Entity()
{
}

Entity::Entity(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer& renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
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
    computeTransformationMatrix();
}

Entity::Entity(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
{
    _renderer = renderer;
    _modelName = modelName;
    _propertiesName = propName;
    _materialName = materialName;
    _materialFile = materialFile;
	static int seed = 1000;

    this->_shader = shader;
    this->_material = _renderer.getObjects()->loadObjMaterial(_materialFile, _materialName);
    this->_properties = _renderer.getObjects()->createProperties(_propertiesName);
    this->_position = pos;
    this->_rotX = rotX;
    this->_rotY = rotY;
    this->_rotZ = rotZ;
    this->_scale = scale;
    computeTransformationMatrix();
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

ShaderPtr Entity::getShader(){
    return _shader;
}

MaterialPtr Entity::getMaterial(){
    return _material;
}

std::string Entity::getMaterialName(){
    return _materialName;
}

PropertiesPtr Entity::getProperties(){
    return _properties;
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

vmml::Matrix4f Entity::getModelMatrix()
{
    return _modelMatrix;
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

double Entity::noise(double nx, double nz)
{
	noise::module::Perlin perlin;
	perlin.SetSeed(_seed);
	perlin.SetOctaveCount(4);
	perlin.SetFrequency(1);
	perlin.SetLacunarity(2.5);

	// Rescale from -1.0:+1.0 to 0.0:1.0
	double value = perlin.GetValue(nx, nz, 0.0) / 2.0 + 0.5;

	// Prevent NaN error by not allowing values below 0
	value = value < 0.0 ? 0.0 : value;

	return value;
}

float Entity::getHeightFromNoise(double nx, double nz)
{
    // Rescale from -1.0:+1.0 to 0.0:1.0
	float res = noise(nx, nz);
    res = pow(res, 1.27);
    res *= 300;
    return res;
}

float Entity::degreeToRadians(float degree) {
    return degree * M_PI/180.0;
}

vmml::Matrix4f Entity::computeTransformationMatrix()
{
    vmml::Matrix4f modelMatrix;
    modelMatrix *= vmml::create_translation(vmml::Vector3f(_position[0], _position[1], _position[2]));
    modelMatrix *= vmml::create_rotation(degreeToRadians(_rotX), vmml::Vector3f::UNIT_X);
    modelMatrix *= vmml::create_rotation(degreeToRadians(_rotY), vmml::Vector3f::UNIT_Y);
    modelMatrix *= vmml::create_rotation(degreeToRadians(_rotZ), vmml::Vector3f::UNIT_Z);
    modelMatrix *= vmml::create_scaling(vmml::Vector3f(_scale));
    this->_modelMatrix = modelMatrix;
    return modelMatrix;
}

void Entity::setSeed(int seed)
{
	_seed = seed;
}
