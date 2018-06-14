#include "Skydome.h"
#include "ProceduralOBJLoader.h"
#include "RenderProject.h"
#include "iostream"
#include "Sun.h"



Skydome::Skydome(std::string modelName, Renderer & renderer)
{
    _model = renderer.getObjects()->getModel(modelName);
    _modelName = modelName;
	_renderer = renderer;

    _skyColor = vmml::Vector3f(0.026, 0.048, 0.096);
    _renderer.getObjects()->getShader("skydome")->setUniform("skydomeSize", _SIZE);
    _renderer.getObjects()->getShader("skydome")->setUniform("skyColor", _skyColor);
    _renderer.getObjects()->getShader("skydome")->setUniform("skydomeDensity", _skydomeDensity);
    _renderer.getObjects()->getShader("skydome")->setUniform("skydomeGradient", _skydomeGradient);

	createClouds();
}

void Skydome::setSkyColor(vmml::Vector3f color)
{
	_skyColor = color;
	_renderer.getObjects()->getShader("skydome")->setUniform("skyColor", color);
}

void Skydome::setSkydomeGradient(float gradient){
    _skydomeGradient = gradient;
    _renderer.getObjects()->getShader("skydome")->setUniform("skydomeGradient", gradient);
}

void Skydome::setSkydomeDensity(float density){
    _skydomeDensity = density;
    _renderer.getObjects()->getShader("skydome")->setUniform("skydomeDensity", density);
}

void Skydome::setSkydomeCenter(vmml::Vector3f center){
    _center = center;
    _renderer.getObjects()->getShader("skydome")->setUniform("skydomeCenter", _center);
}

vmml::Vector3f Skydome::getSkyColor()
{
	return _skyColor;
}

int Skydome::getRandomIntInRange(int lower, int upper)
{
	int random = lower + (rand() % static_cast<int>(upper - lower + 1));
	return random;
}

void Skydome::createClouds()
{
	srand(NULL);
	int numClouds = getRandomIntInRange(5, 20);
	for (int i = 0; i < numClouds; i++)
	{
		int cloudType = getRandomIntInRange(1, 4);
		int scale = getRandomIntInRange(10, 20);
		int height = getRandomIntInRange(0, 100);
		int x = getRandomIntInRange(-200, 200);
		int z = getRandomIntInRange(-200, 200);

		Cloud cloud = {
			vmml::Vector3f(0.0f + (float)x, 350.0f + (float)height, 0.0f + (float)z), // Cloud Position
			scale, // scale
			cloudType // Cloud type [1,4]
		};

		_cloudMap.insert(CloudMap::value_type("Cloud_" + std::to_string(i), cloud));
	}
}

void Skydome::renderClouds(std::string camera, vmml::Vector3f position)
{
	vmml::Matrix4f modelMatrix;
	for (auto const& cloud : _cloudMap) 
	{
		vmml::Vector3f newPos = vmml::Vector3f(position.x() + cloud.second.position.x(), cloud.second.position.y(), position.z() + cloud.second.position.z());
		 modelMatrix =
			vmml::create_translation(newPos) * vmml::create_scaling(vmml::Vector3f(cloud.second.scale));
		_renderer.getObjects()->setAmbientColor(vmml::Vector3f(0.5f));

		_renderer.getModelRenderer()->queueModelInstance("Cloud_"  + std::to_string(cloud.second.type), cloud.first, camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
	}	
}

void Skydome::render(std::string camera, vmml::Vector3f position)
{
    /////// Skydome ///
    vmml::Matrix4f modelMatrix =
    vmml::create_translation(vmml::Vector3f(position.x(), 50.0, position.z())) * vmml::create_scaling(vmml::Vector3f(_scale));
    setSkydomeCenter(vmml::Vector3f(position.x(), _offsetBottom, position.z()));
    _renderer.getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    // draw model
    _renderer.getModelRenderer()->queueModelInstance("skydome", "skydome_instance", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
	renderClouds(camera, position);
}
