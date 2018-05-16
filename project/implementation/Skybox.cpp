#include "Skybox.h"
#include "ProceduralOBJLoader.h"
#include "RenderProject.h"
#include "iostream"


Skybox::Skybox(MaterialPtr material, PropertiesPtr properties)
{
	_material = material;
	_properties = properties;

	// std::cout << "Skybox loaded!!!" << std::endl;
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
