#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"
#include "Tree.h"

class Terrain : public Entity
{
public:
	typedef std::unordered_map< std::string, TreePtr > TreeMap;
    Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, int gridX, int gridZ, int terrain_size, int vertex_count ,vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
    
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	void process(std::string cameraName, const double &deltaTime);
    void render(std::string cameraName);
	void placeTrees();
	TreeMap getTreeMap();
	void renderTrees(std::string camera);
	double noise(double x, double y);
    double getNoiseInput(float coord);
    static float getHeightOfTerrain(float worldX, float worldZ);
    float barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos);
	
    ModelData::GroupMap generate();
    ModelData::GroupMap generateTiledTerrain();
    void generateHeights();
    void generateVertices();
    void generateIdices();
    
    int _TERRAIN_SIZE;
    int _VERTEX_COUNT;

private:
    int _gridX;
    int _gridZ;
    
    float _offsetX;
    float _offsetZ;

	float _exponent;
    float _amplitude;

	int _numFaces;

	

	TreeMap _trees;

    ProceduralOBJLoader _objLoader;
    ModelData::GroupMap _data;
	GroupMap        _groups;
	vmml::AABBf		_boundingBox;


};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */
