#ifndef B_SKYBOX_H
#define B_SKYBOX_H

#include "bRenderer.h"
#include "ProceduralOBJLoader.h"

class Skybox
{
public:
	typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	
	Skybox(MaterialPtr material, PropertiesPtr properties, Renderer & renderer);

	ModelPtr generate();

	void setSkyColor(vmml::Vector3f color);

	vmml::Vector3f getSkyColor();

private:
	MaterialPtr _material;
	PropertiesPtr _properties;
	Renderer _renderer;

	int _numFaces;

	const int SIZE = 900;
    
    ProceduralOBJLoader _objLoader;

	vmml::Vector3f _skyColor;

	GroupMap        _groups;
	vmml::AABBf		_boundingBox;
};
typedef std::shared_ptr< Skybox >  SkyboxPtr;
#endif /* defined(B_SKYBOX_H) */
