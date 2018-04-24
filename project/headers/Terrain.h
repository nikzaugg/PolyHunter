#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"

class Terrain
{
public:
	typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	
	Terrain(MaterialPtr material, PropertiesPtr properties);

	ModelPtr generate();

private:
	MaterialPtr _material;
	PropertiesPtr _properties;
	
	int _VERTEX_COUNT;
	int _SIZE;

	int _numFaces;

	GroupMap        _groups;
	vmml::AABBf		_boundingBox;

	float ** generateHeights();

	float ** _heights;
};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */