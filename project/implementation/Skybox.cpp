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

	// FRONT
	objLoader.addVertex(-1.0, -1.0, 1.0);
	objLoader.addVertex(1.0, -1.0, 1.0);
	objLoader.addVertex(1.0, 1.0, 1.0);
	objLoader.addVertex(-1.0, 1.0, 1.0);

	// BACK
	objLoader.addVertex(-1.0, -1.0, -1.0);
	objLoader.addVertex(1.0, -1.0, -1.0);
	objLoader.addVertex(1.0, 1.0, -1.0);
	objLoader.addVertex(-1.0, 1.0, -1.0);

	// front
	d1.vertexIndex = 2;
	d2.vertexIndex = 1;
	d3.vertexIndex = 0;
	objLoader.addFaceNoTex(d1, d2, d3);
	d1.vertexIndex = 0;
	d2.vertexIndex = 3;
	d3.vertexIndex = 2;
	objLoader.addFaceNoTex(d1, d2, d3);

	// right
	d1.vertexIndex = 6;
	d2.vertexIndex = 5;
	d3.vertexIndex = 1;
	objLoader.addFaceNoTex(d1, d2, d3);
	d1.vertexIndex = 1;
	d2.vertexIndex = 2;
	d3.vertexIndex = 6;
	objLoader.addFaceNoTex(d1, d2, d3);

	// back
	d1.vertexIndex = 5;
	d2.vertexIndex = 6;
	d3.vertexIndex = 7;
	objLoader.addFaceNoTex(d1, d2, d3);
	d1.vertexIndex = 7;
	d2.vertexIndex = 4;
	d3.vertexIndex = 5;
	objLoader.addFaceNoTex(d1, d2, d3);

	// left
	d1.vertexIndex = 3;
	d2.vertexIndex = 0;
	d3.vertexIndex = 4;
	objLoader.addFaceNoTex(d1, d2, d3);
	d1.vertexIndex = 4;
	d2.vertexIndex = 7;
	d3.vertexIndex = 3;
	objLoader.addFaceNoTex(d1, d2, d3);

	// bottom
	d1.vertexIndex = 1;
	d2.vertexIndex = 5;
	d3.vertexIndex = 4;
	objLoader.addFaceNoTex(d1, d2, d3);
	d1.vertexIndex = 4;
	d2.vertexIndex = 0;
	d3.vertexIndex = 1;
	objLoader.addFaceNoTex(d1, d2, d3);

	// top
	d1.vertexIndex = 6;
	d2.vertexIndex = 2;
	d3.vertexIndex = 3;
	objLoader.addFaceNoTex(d1, d2, d3);
	d1.vertexIndex = 3;
	d2.vertexIndex = 7;
	d3.vertexIndex = 6;
	objLoader.addFaceNoTex(d1, d2, d3);


	objLoader.load();

	ModelData::GroupMap data = objLoader.getData();
	ModelPtr skyboxModel = ModelPtr(new Model(data, _material, _properties));
	return skyboxModel;
}
