#include "Terrain.h"
#include "ProceduralOBJLoader.h"

#include "iostream";

int Terrain::_p[] = {};

Terrain::Terrain()
{
	
	//for (int i = 0; i < 512; i++)
	//{
	//	Terrain::p[i] = this->permutation[i % 256];
	//}

	std::cout << "TERRAIN WORKS!!!" << std::endl;

	this->generate();
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

void Terrain::generate()
{
	//ModelData terrainData(true, false);
	ProceduralOBJLoader objLoader;

	vmml::Vector3f center = vmml::Vector3f(0.0f, 0.0f, 0.0f);

	int startPoint = -_SIZE / 2;
	int endPoint = _SIZE / 2;
	
	for (int x = startPoint; x < endPoint; x++)
	{
		for (int z = startPoint; z < endPoint; z++)
		{
			objLoader.addVertex((float)x, 0.0f, (float)z);
		}	
	}

	for (int i = 1; i < _SIZE; i++)
	{
		for (int j = 0; j < _SIZE; j++)
		{
			int v1, v2, v3;

			v1 = i * _SIZE + j - _SIZE;
			v2 = i * _SIZE + j - _SIZE + 1;
			v3 = i * _SIZE + j;

			objLoader.addFace(v1, v2, v3);

			v1 = i * _SIZE + j - _SIZE + 1;
			v2 = i * _SIZE + j;
			v3 = i * _SIZE + j + 1;

			objLoader.addFace(v1, v2, v3);
		}
	}

	objLoader.printFaces();
}