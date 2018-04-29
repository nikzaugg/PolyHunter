#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "math.h";
#include "noise\noise.h"
#include "iostream"

float ** _heights;
using namespace noise;
module::Perlin perlin;

Terrain::Terrain(MaterialPtr material, PropertiesPtr properties, ShaderPtr shader)
{
	_material = material;
	_properties = properties;
	_shader = shader;

	std::cout << "TERRAIN WORKS!!!" << std::endl;
	this->_VERTEX_COUNT = 100;
	this->_SIZE = 500;

	this->_amplitude = 70;
	this->_exponent = 4.18;
	this->_maxHeight = 0.0f;
}

double Terrain::noise(double nx, double ny) {
	// Rescale from -1.0:+1.0 to 0.0:1.0
	return perlin.GetValue(nx, ny, 0) / 2.0 + 0.5;
}

ModelPtr Terrain::generate()
{
	// TODO: Should return a ModelPtr to RenderProject
	//ModelData terrainData(true, false);
	ProceduralOBJLoader objLoader;
	PerlinNoise2D perlinNoise2D;
	PerlinNoise perlinNoise;

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

			float nx = ((float)j / ((float)_VERTEX_COUNT)) -0.5;
			float ny = ((float)i / ((float)_VERTEX_COUNT)) -0.5;

			perlin.SetSeed(549);
			float height = 1 * noise(1 * nx, 1 * ny)
			+0.5 * noise(2 * nx, 2 * ny)
			+0.25 * noise(4 * nx, 4 * ny);

			height = pow(height, _exponent);
			
			_heights[i][j] = height * _amplitude;
	
			if (_maxHeight < _heights[i][j]) 
			{
				_maxHeight = _heights[i][j];
			}
			
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
			d5.vertexIndex = topRight;
			d6.vertexIndex = bottomLeft;
			d4.vertexIndex = bottomRight;
			d4.texCoordsIndex = topRight;
			d5.texCoordsIndex = bottomLeft;
			d6.texCoordsIndex = bottomRight;
			objLoader.addFace(d4, d5, d6);

		}
	}

	_shader->setUniform("amplitude", _amplitude);
	_shader->setUniform("heightPercent", _maxHeight / 100);
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
