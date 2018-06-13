#ifndef B_TERRAIN_H
#define B_TERRAIN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"
#include "Tree.h"
#include "Crystal.h"

class Terrain : public Entity
{
public:
    Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, int gridX, int gridZ, int terrain_size, int vertex_count ,vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);
	
    typedef std::unordered_map< std::string, TreePtr > TreeMap;
    typedef std::unordered_map< std::string, CrystalPtr > CrystalMap;
    typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
    
	void process(std::string cameraName, const double &deltaTime);
    void drawCrystals(std::string camera);
    void renderTerrain(std::string cameraName);
    void customRenderTerrain(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj);
    void customProcess(std::string cameraName, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj);
    TreeMap getTreeMap();
    bool checkCollisionWithEntities(vmml::Vector3f playerPos);
    int getNrOfCrystalsCollected();

	static int seed;
	static double Terrain::noise(double nx, double nz);
	static float Terrain::getHeightFromNoise(double nx, double nz);
	static double Terrain::getNoiseInput(float coord);
	static int TERRAIN_SIZE;
	static int VERTEX_COUNT;

private:
    ModelData::GroupMap generateTerrain();
    void generateTerrainGeometry();
    void placeTree(int i, int j);
    void placeCrystal(int i, int j);
    void processTrees(std::string camera);
    void processCrystals(std::string camera);
    void customProcessTrees(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj);
    void customProcessCrystals(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj);
    
    float barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos);
    
    int _gridX;
    int _gridZ;

    float _offsetX;
    float _offsetZ;

    float _amplitude;

	int _numFaces;

    RendererPtr _renderer;
	TreeMap _trees;
    CrystalMap _crystals;
    static CrystalMap _collectedCrystals;
    int _treeCount = 0;

    ProceduralOBJLoader _objLoader;
    ModelData::GroupMap _data;
	GroupMap        _groups;
	vmml::AABBf		_boundingBox;
};
typedef std::shared_ptr< Terrain >  TerrainPtr;
#endif /* defined(B_TERRAIN_H) */
