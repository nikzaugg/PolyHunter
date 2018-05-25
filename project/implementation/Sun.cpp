#include "Entity.h"
#include "Sun.h"
#include "iostream"
#include "noise.h"
#include <stdlib.h>


Sun::Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
	: Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
	_renderer = renderer;

	// create lights
	_renderer.getObjects()->createLight("sun", vmml::Vector3f(0.0, 200.0, 0.0), vmml::Vector3f(1.0f), vmml::Vector3f(1.0f), 1.0f, 0.5f, 100.0f);
    
	_sunProperties = renderer.getObjects()->createProperties("sun");
	MaterialPtr sunMaterial = renderer.getObjects()->loadObjMaterial("sun.mtl", "sun", shader);
	MaterialPtr moonMaterial = renderer.getObjects()->loadObjMaterial("moon_test.mtl", "moon_test", shader);
	_renderer.getObjects()->createSprite_o("sun", sunMaterial, NO_OPTION, _sunProperties);
	_renderer.getObjects()->createSprite_o("moon_test", moonMaterial, NO_OPTION, renderer.getObjects()->createProperties("moon_test"));
	
	// Add Fragments
	_sunFragments = ModelPtr(new Model(createFragments(), getMaterial(), getProperties()));
	_renderer.getObjects()->addModel("fragment_overlay", _sunFragments);
	setIntensity(0.1f);
}

void Sun::setIntensity(float intensity)
{
	_renderer.getObjects()->getLight("sun")->setIntensity(intensity);
}

void Sun::setPosition(vmml::Vector3f position)
{
	_renderer.getObjects()->getLight("sun")->setPosition(position);
	Entity::setPosition(vmml::Vector3f(position));
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
	vmml::Matrix4f modelMatrix;
	modelMatrix *= vmml::create_translation(vmml::Vector3f(pos.x() - 10.0f, 100.0f, pos.z()));
	modelMatrix *= vmml::create_scaling(vmml::Vector3f(10.f));

	//_renderer.getModelRenderer()->queueModelInstance("fragment_overlay", "fragment_overlay_instance", camera, modelMatrix, std::vector<std::string>({"sun"}), false, false);
}

void Sun::render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD)
{
	// draw model instance
	setPosition(vmml::Vector3f(playerPos.x() + 450.0f, 100.0f, playerPos.z()));
	setScale(50.0f);
	setRotY(90.0f);
	_renderer.getModelRenderer()->queueModelInstance("moon_test", "moon_test_instance", camera, computeTransformationMatrix(), std::vector<std::string>({}), false, false, true, GL_SRC_ALPHA, GL_ONE);
	renderFragments(camera, vmml::Vector3f(playerPos.x() + 450.0f, 100.0f, playerPos.z()));
}
