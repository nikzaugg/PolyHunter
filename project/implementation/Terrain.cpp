#include "Terrain.h"
#include "ProceduralOBJLoader.h"

#include "iostream";

int Terrain::_p[] = {};

Terrain::Terrain(ShaderPtr shader, PropertiesPtr properties)
{
	_properties = properties;

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
	// TODO: Should return a ModelPtr to RenderProject
	//ModelData terrainData(true, false);
	ProceduralOBJLoader objLoader;

	vmml::Vector3f center = vmml::Vector3f(0.0f, 0.0f, 0.0f);

	int startPoint = -_SIZE / 2;
	int endPoint = _SIZE / 2;
	
	for (int x = 0; x < _SIZE; ++x)
	{
		for (int z = 0; z < _SIZE; ++z)
		{
			objLoader.addVertex((float)x, 0.0f, (float)z);
		}	
	}

	for (int i = 1; i < _SIZE; ++i)
	{
		for (int j = 0; j < _SIZE - 1; ++j)
		{
			IndexData d1, d2, d3;

			d1.vertexIndex = i * _SIZE + j - _SIZE;
			d2.vertexIndex = i * _SIZE + j - _SIZE + 1;
			d3.vertexIndex = i * _SIZE + j;

			objLoader.addFace(d1, d2, d3);

			d1.vertexIndex = i * _SIZE + j - _SIZE + 1;
			d2.vertexIndex = i * _SIZE + j;
			d3.vertexIndex = i * _SIZE + j + 1;

			objLoader.addFace(d1, d2, d3);
		}
	}

	objLoader.load();

	ModelData::GroupMap data = objLoader.getData();


	for (auto i = data.begin(); i != data.end(); ++i)
	{
		GeometryPtr g = GeometryPtr(new Geometry);
		_groups.insert(std::pair< std::string, GeometryPtr >(i->first, g));
		GeometryDataPtr gData = i->second;
		g->initialize(gData);
		//g->setMaterial(material);
		//g->setProperties(_properties);

		// expand bounding box
		_boundingBox.merge(g->getBoundingBoxObjectSpace());
	}

	for (auto i = _groups.begin(); i != _groups.end(); ++i)
	{
		i->second->draw(GL_TRIANGLES);
	}	
}