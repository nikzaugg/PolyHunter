#include "Skybox.h"
#include "ProceduralOBJLoader.h"
#include "RenderProject.h"
#include "iostream"
#include "Sun.h"


Skybox::Skybox(MaterialPtr material, PropertiesPtr properties, Renderer & renderer)
{
	_material = material;
	_properties = properties;
	_renderer = renderer;

	// std::cout << "Skybox loaded!!!" << std::endl;
	//Entity(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
	ModelPtr skyBoxModel = generate();
	_renderer.getObjects()->addModel("skybox", skyBoxModel);
	_renderer.getObjects()->getShader("skybox")->setUniform("skyBoxSize", SIZE);
}

ModelPtr Skybox::generate()
{
    IndexData d1, d2, d3;
    
    _objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(-SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(SIZE, -SIZE, -SIZE);
    
    _objLoader.addVertex(SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(-SIZE,  SIZE, -SIZE);

    
    _objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    _objLoader.addVertex(-SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(-SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    
    _objLoader.addVertex(SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(SIZE, -SIZE,  SIZE);
    _objLoader.addVertex(SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(SIZE, -SIZE, -SIZE);
    
    _objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    _objLoader.addVertex(-SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(SIZE, -SIZE,  SIZE);
    _objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    
    _objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(SIZE,  SIZE, -SIZE);
    _objLoader.addVertex(SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(-SIZE,  SIZE,  SIZE);
    _objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    
    _objLoader.addVertex(-SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    _objLoader.addVertex(SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(SIZE, -SIZE, -SIZE);
    _objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    _objLoader.addVertex(SIZE, -SIZE,  SIZE);
    
    int counter = 0;
    for (int i = 0; i<12; i++) {
        d3.vertexIndex = counter++;
        d2.vertexIndex = counter++;
        d1.vertexIndex = counter++;
		_objLoader.addFaceNoTex(d1, d2, d3);
    }


	_objLoader.load();

	ModelData::GroupMap data = _objLoader.getData();
	ModelPtr skyboxModel = ModelPtr(new Model(data, _material, _properties));
	return skyboxModel;
}

void Skybox::setSkyColor(vmml::Vector3f color)
{
	_skyColor = color;
	_renderer.getObjects()->getShader("skybox")->setUniform("skyColor", color);
}

vmml::Vector3f Skybox::getSkyColor()
{
	return _skyColor;
}
