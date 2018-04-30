#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "math.h"
#include "noise.h"
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
	this->_SIZE = 300;

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
	ProceduralOBJLoader objLoader;
	_heights = new float*[_VERTEX_COUNT];

	// noise generation
	for (int  i = 0; i < _VERTEX_COUNT; i++)
	{
		_heights[i] = new float[_VERTEX_COUNT];
		for (int j = 0; j < _VERTEX_COUNT; j++)
		{
			float nx = ((float)j / ((float)_VERTEX_COUNT)) - 0.5;
			float nz = ((float)i / ((float)_VERTEX_COUNT)) - 0.5;

			perlin.SetSeed(549);
			float height = 1 * noise(1 * nx, 1 * nz)
				+ 0.5 * noise(2 * nx, 2 * nz)
				+ 0.25 * noise(4 * nx, 4 * nz);
			
			height = pow(height, _exponent);

			_heights[i][j] = height * _amplitude;

			if (_maxHeight < _heights[i][j])
			{
				_maxHeight = _heights[i][j];
			}
		}
	}

	int counter = 0;
	for (int i = 0; i < _VERTEX_COUNT - 1; i++)
	{
		
		for (int j = 0; j < _VERTEX_COUNT - 1; j++)
		{
			float uPos = (float)j / ((float)_VERTEX_COUNT - 1);
			float vPos = (float)i / ((float)_VERTEX_COUNT - 1);
			uPos = 1 - uPos;
			vPos = 1 - vPos;
			objLoader.addTexCoords(uPos, vPos);

			float xTopLeft = ((float)j / ((float)_VERTEX_COUNT - 1)) * _SIZE;
			float zTopLeft = ((float)i / ((float)_VERTEX_COUNT - 1)) * _SIZE;

			float xTopRight = ((float)(j+1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
			float zTopRight = ((float)i / ((float)_VERTEX_COUNT - 1)) * _SIZE;

			float xBottomLeft = ((float)j / ((float)_VERTEX_COUNT - 1)) * _SIZE;
			float zBottomLeft = ((float)(i+1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;

			float xBottomRight = ((float)(j + 1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
			float zBottomRight = ((float)(i + 1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
			
			objLoader.addVertex(xTopLeft, _heights[i][j], zTopLeft);
			objLoader.addVertex(xBottomLeft, _heights[i+1][j], zBottomLeft);
			objLoader.addVertex(xTopRight, _heights[i][j+1], zTopRight);
			
			IndexData d1, d2, d3;
			d1.vertexIndex = counter++;
			d2.vertexIndex = counter++;
			d3.vertexIndex = counter++;

			objLoader.addFace(d1, d2, d3);

			objLoader.addVertex(xBottomRight, _heights[i+1][j+1], zBottomRight);
			objLoader.addVertex(xTopRight, _heights[i][j+1], zTopRight);
			objLoader.addVertex(xBottomLeft, _heights[i+1][j], zBottomLeft);

			IndexData d4, d5, d6;
			d4.vertexIndex = counter++;
			d5.vertexIndex = counter++;
			d6.vertexIndex = counter++;

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
