#include "Terrain.h"
#include "TerrainLoader.h"
#include "bRenderer.h"
#include <math.h>

TerrainLoader::TerrainLoader(Renderer & renderer, ShaderPtr shader, PlayerPtr player)
{
    this->_player = player;
    this->_renderer = renderer;
    this-> _shader = shader;
    
    // generate initial terrains
    generateTerrain(0, 0);
    generateTerrain(0, 1);
    generateTerrain(1, 0);
    generateTerrain(1, 1);
    generateTerrain(-1, 1);
    generateTerrain(-1, 0);
    generateTerrain(-1, -1);
    generateTerrain(0, -1);
    generateTerrain(1, -1);
    
}

TerrainPtr TerrainLoader::generateTerrain(int gridX, int gridZ)
{
    int xNegative = gridX < 0 ? 1 : 0 ;
    int zNegative = gridZ < 0 ? 1 : 0;
    std::string key = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(gridX) + std::to_string(gridZ);
    std::string terrainName = "terrain" + std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(gridX) + std::to_string(gridZ);
    TerrainPtr terrain = TerrainPtr(new Terrain(terrainName, "terrain.mtl", "terrain", "terrainProperties", _shader, _renderer, gridX, gridZ, _TERRAIN_SIZE, _VERTEX_COUNT, vmml::Vector3f(0.0), 0.0, 0.0, 0.0, 1.0));
    _terrains.insert(TerrainMap::value_type(key , terrain));
    return terrain;
}


void TerrainLoader::process()
{
    // check in which tile the player is
    int gridX = floor(_player->getPosition().x()/_TERRAIN_SIZE);
    int gridZ = floor(_player->getPosition().z()/_TERRAIN_SIZE);

    // std::cout << _player->getPosition().x() << " | " << _player->getPosition().z() << std::endl;
    std::cout <<  " gridX: "<<gridX << " | " << "gridZ: "<< gridZ << std::endl;

    renderTerrains("camera");
}

void TerrainLoader::renderTerrains(std::string camera)
{
    TerrainMap::iterator it;
    for (auto const& x: _terrains) {
        std::cout << "Indices: "<< x.first << " | " <<"Name: "<<  x.second<<  std::endl;
        x.second->render(camera);
    }
}
