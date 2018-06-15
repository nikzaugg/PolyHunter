#include "Entity.h"
#include "Sun.h"
#include "iostream"
#include "noise.h"
#include <stdlib.h>


Sun::Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
	: Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
	_renderer = renderer;
	_shader = shader;


	// create lights
	_lightPosition = vmml::Vector3f(10000, 10000, 0.0);
	_renderer.getObjects()->createLight("sun", _lightPosition, vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 1400.0, 1.0f, 100000.0);
    
    // updates the strength of the sun (between 0 and 1)
    updateSunIntensityInShader("terrain", _sunIntensity);
    updateSunIntensityInShader("basic", _sunIntensity);
    updateSunIntensityInShader("torchLight", _sunIntensity);
}

void Sun::updateSunIntensityInShader(std::string shaderName, float intensity)
{
    _renderer.getObjects()->getShader(shaderName)->setUniform("sunIntensity", intensity);
}

float Sun::getSunIntensity(){
    return _sunIntensity;
}

void Sun::setIntensity(float intensity)
{
	_renderer.getObjects()->getLight("sun")->setIntensity(intensity);
}

void Sun::increaseIntensity(float dI)
{
	float currentIntensity = _renderer.getObjects()->getLight("sun")->getIntensity();
	currentIntensity += dI;
	setIntensity(currentIntensity);
}

void Sun::setPosition(vmml::Vector3f position)
{
	Entity::setPosition(vmml::Vector3f(position));
}

void Sun::setHealth(float health){
    _health = health;
}

void Sun::increaseHealth(float hx){
    _health += hx;
}

float Sun::getHealth(){
    return _health;
}

float Sun::getVertexPos()
{
	noise::module::RidgedMulti ridgedMulti;
	ridgedMulti.SetSeed(99);
	srand(time(NULL));

	return ridgedMulti.GetValue(rand() % 1000000 + 1, rand() % 1000000 + 1, rand() % 1000000 + 1);
}

ModelData::GroupMap Sun::createFragments()
{
	int counter = 0;
	for (int i = 0; i < 3; i++)
	{
		_objLoader.addVertex(getVertexPos(), getVertexPos(), 0.0f);
	}

	IndexData d1, d2, d3;
	d1.vertexIndex = counter++;
	d2.vertexIndex = counter++;
	d3.vertexIndex = counter++;

	_objLoader.addFaceNoTex(d1, d2, d3);

	_objLoader.load();

	return _objLoader.getData();
}

void Sun::renderFragments(std::string camera, vmml::Vector3f pos)
{
	_shader->setUniform("lowerSicknessRange", vmml::Vector2f(0.125, 0.325));
	_shader->setUniform("upperSicknessRange", vmml::Vector2f(0.625, 0.875));

}

void Sun::increaseSunSize(float deltaScale){
    _sunSize += deltaScale;
}

void Sun::render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD, const double &elapsedTime )
{
    if(!_gameHasEnded){
        vmml::Matrix4f modelMatrix = _renderer.getObjects()->getCamera(camera)->getInverseViewMatrix();
        modelMatrix *=
        vmml::create_translation(vmml::Vector3f(0.75f, -0.70f, 0.8f)) *
        vmml::create_scaling(vmml::Vector3f(0.025f));
        modelMatrix *= vmml::create_rotation(float(M_PI_F/10.0 * elapsedTime), vmml::Vector3f::UNIT_Y);
        
        _shaderOffset += 0.2;
        float sinScale = sin(_shaderOffset);
        sinScale =  (sinScale - (-1.0)) * (_pulsateMax - _pulsateMin) / (1.0 - (-1.0)) + _pulsateMin;
        
        getShader()->setUniform("time", sinScale);
        getShader()->setUniform("sickness", _health);
        getShader()->setUniform("doVanish", 0.0);
        getShader()->setUniform("vanish", 0.0);
        modelMatrix *= vmml::create_scaling(vmml::Vector3f(_sunSize * sinScale));
        _renderer.getModelRenderer()->queueModelInstance("sun", "sun_instance", camera, modelMatrix, std::vector<std::string>({ "sun", "torch" }));
    }
}

void Sun::startEndGameAnimation(const double &deltaTime, const double &elapsedTime, SkydomePtr skydome)
{
    _gameHasEnded = true;
    _animationTimer += 0.1;
    float lazy = 0.95;
    
    vmml::Vector3f nextPosition;
    vmml::Matrix4f inverseView = _renderer.getObjects()->getCamera("camera")->getInverseViewMatrix();
    vmml::Vector3f targetPosition = vmml::Vector3f(0.0f, 0.0f, 0.8f);
    
    if (!_positionReached) {
        nextPosition.x() = (lazy * _screenSunPosition.x()) + ((1.0-lazy) * targetPosition.x());
        nextPosition.y() = (lazy * _screenSunPosition.y()) + ((1.0-lazy) * targetPosition.y());
        nextPosition.z() = (lazy * _screenSunPosition.z()) + ((1.0-lazy) * targetPosition.z());
        _screenSunPosition = nextPosition;
        
        vmml::Vector3f distance = targetPosition - _screenSunPosition;
        float dist = distance.squared_length();
        std::cout << dist << std::endl;
        _positionReached =  dist < 0.0001 ? true : false;
    } else if(_positionReached) {
        _screenSunPosition = targetPosition;
        nextPosition = _screenSunPosition;
        if(_sunSize < 2.0){
            _sunSize += 0.02;
        }
        if(_sunSize >= 2.0){
            _sizeReached = true;
        }
    }
    if(_sizeReached) {
        _renderer.getObjects()->getShader("sun")->setUniform("doVanish", 1.0);
        _renderer.getObjects()->getShader("sun")->setUniform("vanish", 1.0);

        vmml::Vector3f skyColor = vmml::Vector3f(0.26, 0.48, 0.96) * 1.2;
        vmml::Vector3f fogColor = vmml::Vector3f(0.8);
        
        ShaderPtr basic = _renderer.getObjects()->getShader("basic");
        ShaderPtr terrain = _renderer.getObjects()->getShader("terrain");
        basic->setUniform("fogDensity", 0.0025);
        basic->setUniform("fogGradient", 20.0);
        basic->setUniform("fogColor", skyColor);
        terrain->setUniform("fogDensity", 0.0025);
        terrain->setUniform("fogGradient", 20.0);
        terrain->setUniform("fogColor", skyColor);
        
        skydome->setSkyColor(skyColor);
        
        updateSunIntensityInShader("terrain", 1.0);
        updateSunIntensityInShader("basic", 1.0);
        updateSunIntensityInShader("torchLight", 1.0);
        
        _renderer.getObjects()->getLight("torch")->setIntensity(1.0);
    }
    
    vmml::Matrix4f modelMatrix = inverseView *
    vmml::create_translation(nextPosition) *
    vmml::create_scaling(vmml::Vector3f(_sunSize * _sunScale));
    
    _renderer.getModelRenderer()->queueModelInstance("sun", "sun_instance", "camera", modelMatrix, std::vector<std::string>({ "sun", "torch" }));
}

void Sun::reset(){
    _health = 0.0;
    _sunIntensity = 0.3;
    _shaderOffset = 0.0;
    _sunSize = 1.0;
    _sunScale = 0.025;
    _pulsateMin = 0.9;
    _pulsateMax = 1.0;
    _animationTimer = 0.0;
    _gameHasEnded = false;
    _positionReached = false;
    _sizeReached = false;
    _vanishFactor = 0.0;
    _screenSunPosition = vmml::Vector3f(0.75f, -0.70f, 0.8f);
    // updates the strength of the sun (between 0 and 1)
    updateSunIntensityInShader("terrain", _sunIntensity);
    updateSunIntensityInShader("basic", _sunIntensity);
    updateSunIntensityInShader("torchLight", _sunIntensity);
    _renderer.getObjects()->getLight("torch")->setIntensity(1400);
}
