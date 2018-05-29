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
    void setSkyboxGradient(float gradient);
    void setSkyboxDensity(float density);

	vmml::Vector3f getSkyColor();

private:
	MaterialPtr _material;
	PropertiesPtr _properties;
	Renderer _renderer;

	int _numFaces;

    vmml::Vector3f _skyColor;
	const int SIZE = 900;
    float _skyBoxGradient = 0.04;
    float _skyBoxDensity = 0.9;
    
    ProceduralOBJLoader _objLoader;

	GroupMap        _groups;
	vmml::AABBf		_boundingBox;
};
typedef std::shared_ptr< Skybox >  SkyboxPtr;
#endif /* defined(B_SKYBOX_H) */
