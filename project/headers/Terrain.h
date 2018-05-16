#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"
#include "Tree.h"

class Terrain : public Entity
{
public:
    Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, int gridX, int gridZ, int terrain_size, int vertex_count ,vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
	
    typedef std::unordered_map< std::string, TreePtr > TreeMap;
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
    
	void process(std::string cameraName, const double &deltaTime);
    TreeMap getTreeMap();

    int _TERRAIN_SIZE;
    int _VERTEX_COUNT;

private:
    ModelData::GroupMap generateTerrain();
    void generateTerrainGeometry();
    void placeTree(int i, int j);
    void renderTerrain(std::string cameraName);
    void processTrees(std::string camera);
    double getNoiseInput(float coord);
    
    float barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos);
    
    int _gridX;
    int _gridZ;

    float _offsetX;
    float _offsetZ;

    float _amplitude;

	int _numFaces;

	TreeMap _trees;
    int _treeCount = 0;

    ProceduralOBJLoader _objLoader;
    ModelData::GroupMap _data;
	GroupMap        _groups;
	vmml::AABBf		_boundingBox;
};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */
