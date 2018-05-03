#ifndef B_SKYBOX_H
#define B_SKYBOX_H

#include "bRenderer.h"

class Skybox
{
public:
	typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	
	Skybox(MaterialPtr material, PropertiesPtr properties);

	ModelPtr generate();

private:
	MaterialPtr _material;
	PropertiesPtr _properties;

	int _numFaces;
	const int SIZE = 150;

	GroupMap        _groups;
	vmml::AABBf		_boundingBox;
};
typedef std::shared_ptr< Skybox >  SkyboxPtr;
#endif /* defined(B_SKYBOX_H) */
