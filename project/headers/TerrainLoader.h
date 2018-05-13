#ifndef B_TERRAINLOADER_H
#define B_TERRAINLOADER_H

#include "bRenderer.h"
#include "Terrain.h"
#include "Player.h"

class TerrainLoader {
    
public:
	typedef std::unordered_map< std::string, TerrainPtr > TerrainMap;
    TerrainLoader(Renderer & renderer, ShaderPtr shader, PlayerPtr player);
    void renderTerrains(std::string camera);
	TerrainLoader::TerrainMap getTerrainMap();
    void process();
private:
    TerrainPtr generateTerrain(int gridX, int gridZ);
    void refreshTerrainTiles();
    
    TerrainMap _terrains;
    PlayerPtr _player;
    ShaderPtr _shader;
    Renderer _renderer;
    int _nrOfTerrains;
    vmml::Vector3f _centerPoint;
    
    int _TERRAIN_SIZE = 150;
    int _VERTEX_COUNT = 30;
    
    int _terrainXPlayer;
    int _terrainZPlayer;
    
};
typedef std::shared_ptr< TerrainLoader >  TerrainLoaderPtr;
#endif /* defined(B_TERRAINLOADER_H) */

