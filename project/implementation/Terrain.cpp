#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "math.h";

#include "iostream"

float ** _heights;

Terrain::Terrain(MaterialPtr material, PropertiesPtr properties)
{
	_material = material;
	_properties = properties;

	std::cout << "TERRAIN WORKS!!!" << std::endl;
	this->_VERTEX_COUNT = 100;
	this->_SIZE = 300;


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
			float uPos = (float)j / ((float)_VERTEX_COUNT - 1);
			float vPos = (float)i / ((float)_VERTEX_COUNT - 1);
			uPos = 1 - uPos;
			vPos = 1 - vPos;
			objLoader.addTexCoords(uPos, vPos);

			float xPos = ((float)j / ((float)_VERTEX_COUNT - 1)) * _SIZE;
			float zPos = ((float)i / ((float)_VERTEX_COUNT - 1)) * _SIZE;

			float nx = j / (float)_SIZE;
			float ny = i / (float)_SIZE;

			float height = 1 * perlinNoise.perlin(1.75f * nx, 1.75f * ny)
				+ 0.5 * perlinNoise.perlin(2 * nx, 2 * ny) 
				+ 0.25 * perlinNoise.perlin(4 * nx, 2 * ny);

			_heights[i][j] = height * 100;

			objLoader.addVertex(xPos, _heights[i][j], zPos);
		}
	}

	for (int i = 0; i < _VERTEX_COUNT - 1; i++) {
		for (int j = 0; j < _VERTEX_COUNT - 1; j++) {

			long int topLeft = (i*_VERTEX_COUNT) + j;
			long int topRight = topLeft + 1;
			long int bottomLeft = ((i + 1)*_VERTEX_COUNT) + j;
			long int bottomRight = bottomLeft + 1;

			IndexData d1, d2, d3;
			d1.vertexIndex = topLeft;
			d2.vertexIndex = bottomLeft;
			d3.vertexIndex = topRight;
			d1.texCoordsIndex = topLeft;
			d2.texCoordsIndex = bottomLeft;
			d3.texCoordsIndex = topRight;
			objLoader.addFace(d1, d2, d3);

			IndexData d4, d5, d6;
			d4.vertexIndex = topRight;
			d5.vertexIndex = bottomLeft;
			d6.vertexIndex = bottomRight;
			d4.texCoordsIndex = topRight;
			d5.texCoordsIndex = bottomLeft;
			d6.texCoordsIndex = bottomRight;
			objLoader.addFace(d4, d5, d6);

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
