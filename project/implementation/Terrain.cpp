#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"

#include "iostream"

float ** _heights;

Terrain::Terrain(MaterialPtr material, PropertiesPtr properties)
{
	_material = material;
	_properties = properties;

	std::cout << "TERRAIN WORKS!!!" << std::endl;

}

ModelPtr Terrain::generate()
{
	// TODO: Should return a ModelPtr to RenderProject
	//ModelData terrainData(true, false);
	ProceduralOBJLoader objLoader;
	PerlinNoise2D perlinNoise;

	//generateHeights();

	vmml::Vector3f center = vmml::Vector3f(0.0f, 0.0f, 0.0f);

	_heights = new float*[_VERTEX_COUNT];
	
	for (int i = 0; i < _VERTEX_COUNT; i++)
	{
		_heights[i] = new float[_VERTEX_COUNT];
		for (int j = 0; j < _VERTEX_COUNT; j++)
		{
			float uPos = (float)i / ((float)_VERTEX_COUNT/2.0 - 1);
			float vPos = (float)j / ((float)_VERTEX_COUNT/2.0 - 1);
			objLoader.addTexCoords(uPos, vPos);

			float xPos = (float)j / ((float)_VERTEX_COUNT - 1) * _SIZE;
			float zPos = (float)i / ((float)_VERTEX_COUNT - 1) * _SIZE;
			_heights[i][j] = perlinNoise.generateHeight(xPos, zPos);

			objLoader.addVertex(xPos, _heights[i][j], zPos);
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
			d1.texCoordsIndex = topLeft;
			d2.texCoordsIndex = bottomLeft;
			d3.texCoordsIndex = topRight;
			objLoader.addFace(d1, d2, d3);

			d1.vertexIndex = topRight;
			d2.vertexIndex = bottomLeft;
			d3.vertexIndex = bottomRight;
			d1.texCoordsIndex = topRight;
			d2.texCoordsIndex = bottomLeft;
			d3.texCoordsIndex = bottomRight;
			objLoader.addFace(d1, d2, d3);
		}
	}

	objLoader.load();

	ModelData::GroupMap data = objLoader.getData();
	ModelPtr terrainModel = ModelPtr(new Model(data, _material, _properties));

	return terrainModel;
}

float** Terrain::generateHeights()
{
	PerlinNoise2D perlinNoise;
	float ** heights = new float*[_VERTEX_COUNT];

	for (int z = 0; z < _VERTEX_COUNT; z++)
	{
		for (int x = 0; x < _VERTEX_COUNT; x++)
		{
			perlinNoise.generateHeight((float)x, (float)z);
		}
	}


	return heights;
}
