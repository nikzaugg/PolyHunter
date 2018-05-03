#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"

class Terrain : public Entity
{
public:
    Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
    
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	void process(std::string cameraName, const double &deltaTime);
    void render(std::string cameraName);
	double noise(double x, double y);
    float getHeightOfTerrain(float worldX, float worldZ);
    float barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos);
	ModelData::GroupMap generate();
    void generateHeights();
    void generateVertices();
    void generateIdices();

private:

	float _exponent;
	float _amplitude;
    
    int _TERRAIN_SIZE;
    int _VERTEX_COUNT;

	int _numFaces;

    ProceduralOBJLoader _objLoader;
    ModelData::GroupMap _data;
	GroupMap        _groups;
	vmml::AABBf		_boundingBox;

    float ** _heights;

	float _maxHeight;
};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */
