#include "Terrain.h"
#include "TerrainLoader.h"
#include "bRenderer.h"
#include <math.h>
#include <tuple>

TerrainLoader::TerrainLoader(Renderer & renderer, ShaderPtr shader, CamPtr playerCam)
{
    // initial tile position of the player
    _terrainXPlayer = 0;
    _terrainZPlayer = 0;
    
    this->_player = playerCam;
    this->_renderer = renderer;
    this-> _shader = shader;
	this->_shader->setUniform("skyColor", vmml::Vector3f(0.5f, 0.5f, 0.5f));
    
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

void TerrainLoader::process(std::string camera, const double &deltaTime)
{
    float playerX = -1.0 * _player->getPosition().x();
    float playerZ = -1.0 * _player->getPosition().z();
    // check in which tile the player is
    int gridX = floor(playerX/_TERRAIN_SIZE);
    int gridZ = floor(playerZ/_TERRAIN_SIZE);
    
    std::cout << "computed GridX" << gridX << std::endl;
    std::cout << "computed GridZ" << gridZ << std::endl;
    
    std::cout << "_terrainXPlayer: " << _terrainXPlayer << std::endl;
    std::cout << "_terrainZPlayer: " << _terrainZPlayer << std::endl;

    // check if tile has changed
    if((gridX != _terrainXPlayer) ||(gridZ != _terrainZPlayer)){
        std::cout <<  "CHANGED TILES!!!!!!!!!!" << std::endl;
        _terrainXPlayer = gridX;
        _terrainZPlayer = gridZ;
        refreshTerrainTiles();
    }

    renderTerrains(camera, deltaTime);
}

void TerrainLoader::customProcess(std::string camera, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    float playerX = -1.0 * _player->getPosition().x();
    float playerZ = -1.0 * _player->getPosition().z();
    // check in which tile the player is
    int gridX = floor(playerX/_TERRAIN_SIZE);
    int gridZ = floor(playerZ/_TERRAIN_SIZE);
    
    // check if tile has changed
    if((gridX != _terrainXPlayer) ||(gridZ != _terrainZPlayer)){
        std::cout <<  "CHANGED TILES!!!!!!!!!!" << std::endl;
        _terrainXPlayer = gridX;
        _terrainZPlayer = gridZ;
        refreshTerrainTiles();
    }
    
    customRenderTerrains(camera, deltaTime, view, proj);
}

void TerrainLoader::refreshTerrainTiles()
{
    // generate new map
    TerrainMap newTerrainMap;
    newTerrainMap.clear();
    // new terrain keys (string identifiers)
    std::vector<std::string> newTerrainKeys;
    
    // new tuple vector for holding indices
    typedef std::vector<std::tuple<int, int>> TerrainIndexPairs;
    TerrainIndexPairs terrainIndexPairs;
    
    // find out which indices should exist
    // create names and indices of the 9 terrain tiles that should exist in the next frame
    // CENTER (where player is right now)
    int xNegative = _terrainXPlayer < 0 ? 1 : 0;
    int zNegative = _terrainZPlayer < 0 ? 1 : 0;
    std::string key0 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer) + std::to_string(_terrainZPlayer);
    newTerrainKeys.push_back(key0);
    auto t = std::make_tuple(_terrainXPlayer, _terrainZPlayer);
    terrainIndexPairs.push_back(t);
    
    // UP
    xNegative = _terrainXPlayer < 0 ? 1 : 0;
    zNegative = _terrainZPlayer+1 < 0 ? 1 : 0;
    std::string key1 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer) + std::to_string(_terrainZPlayer+1);
    newTerrainKeys.push_back(key1);
    t = std::make_tuple(_terrainXPlayer, _terrainZPlayer+1);
    terrainIndexPairs.push_back(t);
    
    // DOWN
    xNegative = _terrainXPlayer < 0 ? 1 : 0;
    zNegative = _terrainZPlayer-1 < 0 ? 1 : 0;
    std::string key2 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer) + std::to_string(_terrainZPlayer-1);
    newTerrainKeys.push_back(key2);
    t = std::make_tuple(_terrainXPlayer, _terrainZPlayer-1);
    terrainIndexPairs.push_back(t);
    
    // LEFT
    xNegative = _terrainXPlayer-1 < 0 ? 1 : 0;
    zNegative = _terrainZPlayer < 0 ? 1 : 0;
    std::string key3 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer-1) + std::to_string(_terrainZPlayer);
    newTerrainKeys.push_back(key3);
    t = std::make_tuple(_terrainXPlayer-1, _terrainZPlayer);
    terrainIndexPairs.push_back(t);
    
    // RIGHT
    xNegative = _terrainXPlayer+1 < 0 ? 1 : 0;
    zNegative = _terrainZPlayer < 0 ? 1 : 0;
    std::string key4 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer+1) + std::to_string(_terrainZPlayer);
    newTerrainKeys.push_back(key4);
    t = std::make_tuple(_terrainXPlayer+1, _terrainZPlayer);
    terrainIndexPairs.push_back(t);
    
    // UPPER-LEFT
    xNegative = _terrainXPlayer-1 < 0 ? 1 : 0;
    zNegative = _terrainZPlayer+1 < 0 ? 1 : 0;
    std::string key5 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer-1) + std::to_string(_terrainZPlayer+1);
    newTerrainKeys.push_back(key5);
    t = std::make_tuple(_terrainXPlayer-1, _terrainZPlayer+1);
    terrainIndexPairs.push_back(t);
    
    // BOTTOM-LEFT
    xNegative = _terrainXPlayer-1 < 0 ? 1 : 0;
    zNegative = _terrainZPlayer-1 < 0 ? 1 : 0;
    std::string key6 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer-1) + std::to_string(_terrainZPlayer-1);
    newTerrainKeys.push_back(key6);
    t = std::make_tuple(_terrainXPlayer-1, _terrainZPlayer-1);
    terrainIndexPairs.push_back(t);
    
    // TOP-RIGHT
    xNegative = _terrainXPlayer+1 < 0 ? 1 : 0;
    zNegative = _terrainZPlayer+1 < 0 ? 1 : 0;
    std::string key7 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer+1) + std::to_string(_terrainZPlayer+1);
    newTerrainKeys.push_back(key7);
    t = std::make_tuple(_terrainXPlayer+1, _terrainZPlayer+1);
    terrainIndexPairs.push_back(t);
    
    // BOTTOM-RIGHT
    xNegative = _terrainXPlayer+1 < 0 ? 1 : 0;
    zNegative = _terrainZPlayer-1 < 0 ? 1 : 0;
    std::string key8 = std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(_terrainXPlayer+1) + std::to_string(_terrainZPlayer-1);
    newTerrainKeys.push_back(key8);
    t = std::make_tuple(_terrainXPlayer+1, _terrainZPlayer-1);
    terrainIndexPairs.push_back(t);
    
    // compare and rearrange current terrain map and decide which new terrains should be rendered
    for (int i = 0; i<9; i++) {
        if(_terrains.find(newTerrainKeys[i]) == _terrains.end()){
            // DOES NOT exist already (create TerrainPtr and add it)
            std::cout << "have to generate tile: " << newTerrainKeys[i] << std::endl;
            std::string terrainName = "terrain" + newTerrainKeys[i];
            std::tuple<int, int> indexTuple = terrainIndexPairs[i];
            int x_Index = std::get<0>(indexTuple);
            int z_Index= std::get<1>(indexTuple);
            TerrainPtr terrain = generateTerrain(x_Index, z_Index);
            newTerrainMap.insert(TerrainMap::value_type(newTerrainKeys[i] , terrain));
            
        } else {
            // DOES EXIST already (add existing pointer to new map)
            newTerrainMap[newTerrainKeys[i]] = _terrains[newTerrainKeys[i]];
        }
    }
    
    // delete old map which only contains terrain tiles that are not needed anymore
    // reassign new map to old map variable
    _terrains = newTerrainMap;
    std::cout << _terrains.size() << std::endl;
}

void TerrainLoader::renderCrystals(std::string camera, const double &deltaTime)
{
    TerrainMap::iterator it;
    for (auto const& x: _terrains) {
        x.second->drawCrystals(camera);
    }
}

void TerrainLoader::renderTerrains(std::string camera, const double &deltaTime)
{
    TerrainMap::iterator it;
    for (auto const& x: _terrains) {
        x.second->process(camera, deltaTime);
    }
}

void TerrainLoader::customRenderTerrains(std::string camera, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    TerrainMap::iterator it;
    for (auto const& x: _terrains) {
        x.second->customProcess(camera, deltaTime, view, proj);
    }
}

TerrainLoader::TerrainMap TerrainLoader::getTerrainMap()
{
	return this->_terrains;
}
