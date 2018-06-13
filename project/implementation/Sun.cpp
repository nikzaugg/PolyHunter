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
	_lightPosition = vmml::Vector3f(300, 300, 0.0);
	_renderer.getObjects()->createLight("sun", _lightPosition, vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 1400.0, 1.0f, 100000.0);
	setPosition(vmml::Vector3f(_lightPosition));
	this->setIntensity(1.0f);
    
    // updates the strength of the sun (between 0 and 1)
    updateSunIntensityInShader("terrain", _sunIntensity);
    updateSunIntensityInShader("basic", _sunIntensity);
    updateSunIntensityInShader("torchLight", _sunIntensity);

 
	//_sunProperties = renderer.getObjects()->createProperties("sun");
	//MaterialPtr sunMaterial = renderer.getObjects()->loadObjMaterial("sun.mtl", "sun", _shader);
	//MaterialPtr sunMaterial = renderer.getObjects()->loadObjMaterial("sun.mtl", "sun", _shader);
	//_renderer.getObjects()->createSprite_o("sun", sunMaterial, NO_OPTION, _sunProperties);

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

void Sun::render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD)
{
	_lightPosition = vmml::Vector3f(playerPos.x() + 1200, 1000, playerPos.z());
    _renderer.getObjects()->getLight("sun")->setPosition(_lightPosition);

	// draw model instance
	setPosition(playerPos + (_lightPosition - playerPos) * 0.3f);

	//setScale(5.0f);
	setRotY(90.0f);

	//_renderer.getModelRenderer()->queueModelInstance("sun", "sun_instance", camera, computeTransformationMatrix(), std::vector<std::string>({}), false, false, true, GL_SRC_ALPHA, GL_ONE);
	_renderer.getModelRenderer()->queueModelInstance("sun", "sun_instance", camera, computeTransformationMatrix(), std::vector<std::string>({}), false, false, true);
}
