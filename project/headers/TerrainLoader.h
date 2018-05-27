#ifndef B_TERRAINLOADER_H
#define B_TERRAINLOADER_H

#include "bRenderer.h"
#include "Terrain.h"
#include "Player.h"
#include "Cam.h"

class TerrainLoader {

public:
	typedef std::unordered_map< std::string, TerrainPtr > TerrainMap;
    TerrainLoader(Renderer & renderer, ShaderPtr shader, CamPtr playerCam);
    void renderTerrains(std::string camera, const double &deltaTime);
    void renderCrystals(std::string camera, const double &deltaTime);
    void customRenderTerrains(std::string camera, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj);
	TerrainLoader::TerrainMap getTerrainMap();
    void process(std::string camera, const double &deltaTime);
    void customProcess(std::string camera, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj);
    int getPlayerGridX();
    int getPlayerGridZ();
    std::string generateTerrainKey(int x, int z);
    std::string generateTerrainName(int x, int z);
    TerrainPtr getSingleTerrain(std::string terrainKey);
private:
    TerrainPtr generateTerrain(int gridX, int gridZ);
    void refreshTerrainTiles();
    TerrainMap _terrains;
    ShaderPtr _shader;
    CamPtr _player;
    Renderer _renderer;
    int _nrOfTerrains;
    vmml::Vector3f _centerPoint;
    
    int _TERRAIN_SIZE = 200;
    int _VERTEX_COUNT = 15;
    
    int _terrainXPlayer;
    int _terrainZPlayer;

};
typedef std::shared_ptr< TerrainLoader >  TerrainLoaderPtr;
#endif /* defined(B_TERRAINLOADER_H) */
