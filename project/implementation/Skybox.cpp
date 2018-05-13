#include "Skybox.h"
#include "ProceduralOBJLoader.h"
#include "RenderProject.h"
#include "iostream"


Skybox::Skybox(MaterialPtr material, PropertiesPtr properties)
{
	_material = material;
	_properties = properties;

	std::cout << "Skybox loaded!!!" << std::endl;
}

ModelPtr Skybox::generate()
{
	ProceduralOBJLoader objLoader;
    
    IndexData d1, d2, d3;
    
    objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    objLoader.addVertex(-SIZE, -SIZE, -SIZE);
    objLoader.addVertex(SIZE, -SIZE, -SIZE);
    
    objLoader.addVertex(SIZE, -SIZE, -SIZE);
    objLoader.addVertex(SIZE,  SIZE, -SIZE);
    objLoader.addVertex(-SIZE,  SIZE, -SIZE);

    
    objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    objLoader.addVertex(-SIZE, -SIZE, -SIZE);
    objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    objLoader.addVertex(-SIZE,  SIZE,  SIZE);
    objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    
    objLoader.addVertex(SIZE, -SIZE, -SIZE);
    objLoader.addVertex(SIZE, -SIZE,  SIZE);
    objLoader.addVertex(SIZE,  SIZE,  SIZE);
    objLoader.addVertex(SIZE,  SIZE,  SIZE);
    objLoader.addVertex(SIZE,  SIZE, -SIZE);
    objLoader.addVertex(SIZE, -SIZE, -SIZE);
    
    objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    objLoader.addVertex(-SIZE,  SIZE,  SIZE);
    objLoader.addVertex(SIZE,  SIZE,  SIZE);
    objLoader.addVertex(SIZE,  SIZE,  SIZE);
    objLoader.addVertex(SIZE, -SIZE,  SIZE);
    objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    
    objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    objLoader.addVertex(SIZE,  SIZE, -SIZE);
    objLoader.addVertex(SIZE,  SIZE,  SIZE);
    objLoader.addVertex(SIZE,  SIZE,  SIZE);
    objLoader.addVertex(-SIZE,  SIZE,  SIZE);
    objLoader.addVertex(-SIZE,  SIZE, -SIZE);
    
    objLoader.addVertex(-SIZE, -SIZE, -SIZE);
    objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    objLoader.addVertex(SIZE, -SIZE, -SIZE);
    objLoader.addVertex(SIZE, -SIZE, -SIZE);
    objLoader.addVertex(-SIZE, -SIZE,  SIZE);
    objLoader.addVertex(SIZE, -SIZE,  SIZE);
    
    int counter = 0;
    for (int i = 0; i<12; i++) {
        d3.vertexIndex = counter++;
        d2.vertexIndex = counter++;
        d1.vertexIndex = counter++;
		if (Input::isTouchDevice()) {
			objLoader.addFaceNoTex(d3, d2, d1);
		}
		else {
			objLoader.addFaceNoTex(d3, d2, d1);
		}
       
    }


	objLoader.load();

	ModelData::GroupMap data = objLoader.getData();
	ModelPtr skyboxModel = ModelPtr(new Model(data, _material, _properties));
	return skyboxModel;
}
