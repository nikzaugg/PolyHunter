#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"
#include "Entity.h"

class Terrain : public Entity
{
public:
    Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
    
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	void process(std::string cameraName, const double &deltaTime);
    void render(std::string cameraName);
	double noise(double x, double y);

	ModelData::GroupMap generate();

private:
	int _VERTEX_COUNT;
	int _SIZE;

	float _exponent;
	float _amplitude;

	int _numFaces;

    ModelData::GroupMap _data;
	GroupMap        _groups;
	vmml::AABBf		_boundingBox;

	float ** _heights;

	float _maxHeight;
};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */
