#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"

class Terrain
{
public:
	typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	
	Terrain(MaterialPtr material, PropertiesPtr properties);

	Terrain(MaterialPtr material, PropertiesPtr properties, ShaderPtr shader);

	double noise(double x, double y);

	ModelPtr generate();

private:
	MaterialPtr _material;
	PropertiesPtr _properties;
	ShaderPtr _shader;
	
	int _VERTEX_COUNT;
	int _SIZE;

	float _exponent;
	float _amplitude;

	int _numFaces;

	GroupMap        _groups;
	vmml::AABBf		_boundingBox;

	float ** generateHeights();

	float ** _heights;

	float _maxHeight;
};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */