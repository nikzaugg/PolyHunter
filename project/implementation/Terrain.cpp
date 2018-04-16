#include "Terrain.h"
#include "ProceduralOBJLoader.h"

#include "iostream";

int Terrain::_p[] = {};

Terrain::Terrain(MaterialPtr material, PropertiesPtr properties)
{
	_material = material;
	_properties = properties;

	std::cout << "TERRAIN WORKS!!!" << std::endl;
}

float Terrain::perlin(float x, float y, float z)
{
	// Calculate the "unit cube" that the point asked will be located in
	// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	int xi = (int)x & 255;								
	int yi = (int)y & 255;								
	int zi = (int)z & 255;								
	float xf = x - (int)x;

	return 1.0f;
}

ModelPtr Terrain::generate()
{
	// TODO: Should return a ModelPtr to RenderProject
	//ModelData terrainData(true, false);
	ProceduralOBJLoader objLoader;

	vmml::Vector3f center = vmml::Vector3f(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < _VERTEX_COUNT; i++)
	{
		for (int j = 0; j < _VERTEX_COUNT; j++)
		{
			float xPos = (float)j / ((float)_VERTEX_COUNT - 1) * _SIZE;
			float yPos = ((float)rand()*3.0 / (RAND_MAX));
			float zPos = (float)i / ((float)_VERTEX_COUNT - 1) * _SIZE;
			objLoader.addVertex(xPos, yPos, zPos);
		}
	}

	for (int gz = 0; gz<_VERTEX_COUNT /2 - 1; ++gz) {
		for (int gx = 0; gx<_VERTEX_COUNT /2 - 1; ++gx) {
			int topLeft = (gz*_VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*_VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			IndexData d1, d2, d3;
			d1.vertexIndex = topLeft;
			d2.vertexIndex = bottomLeft;
			d3.vertexIndex = topRight;
			objLoader.addFace(d1, d2, d3);
			
			IndexData d11, d22, d33;
			d11.vertexIndex = topRight;
			d22.vertexIndex = bottomLeft;
			d33.vertexIndex = bottomRight;
			objLoader.addFace(d11, d22, d33);
		}
	}

	objLoader.load();

	ModelData::GroupMap data = objLoader.getData();
	ModelPtr terrainModel = ModelPtr(new Model(data, _material, _properties));

	return terrainModel;
}