#include "Terrain.h"
#include "TerrainLoader.h"
#include "bRenderer.h"
#include "Entity.h"
#include <math.h>
#include <tuple>
#include "noise.h"

TerrainLoader::TerrainLoader(Renderer & renderer, ShaderPtr shader, CamPtr playerCam)
{
    // initial tile position of the player
    _terrainXPlayer = 0;
    _terrainZPlayer = 0;
    
    this->_player = playerCam;
    this->_renderer = renderer;
    this-> _shader = shader;
	srand(time(NULL));
	this->_seed = 10 + (rand() % static_cast<int>(10000 - 10 + 1));
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
    std::string key = generateTerrainKey(gridX, gridZ);
    std::string terrainName = generateTerrainName(gridX, gridZ);
    TerrainPtr terrain = TerrainPtr(new Terrain(terrainName, "terrain.mtl", "terrain", "terrainProperties", _shader, _renderer, gridX, gridZ, _TERRAIN_SIZE, _VERTEX_COUNT, vmml::Vector3f(0.0), 0.0, 0.0, 0.0, 1.0));
    _terrains.insert(TerrainMap::value_type(key , terrain));
    return terrain;
}

int TerrainLoader::getPlayerGridX()
{
    return _terrainXPlayer;
}

int TerrainLoader::getPlayerGridZ()
{
    return _terrainZPlayer;
}

std::string TerrainLoader::generateTerrainKey(int x, int z)
{
    int xNegative = x < 0 ? 1 : 0 ;
    int zNegative = z < 0 ? 1 : 0;
    return std::to_string(xNegative) + std::to_string(zNegative) + std::to_string(x) + std::to_string(z);
}

std::string TerrainLoader::generateTerrainName(int x, int z)
{
    int xNegative = x < 0 ? 1 : 0 ;
    int zNegative = z < 0 ? 1 : 0;
    return "terrain" + std::to_string(xNegative)  + std::to_string(zNegative) + std::to_string(x) + std::to_string(z);
}

void TerrainLoader::process(std::string camera, const double &deltaTime)
{
    float playerX = _player->getPosition().x();
    float playerZ = _player->getPosition().z();
    // check in which tile the player is
    int gridX = floor(playerX/_TERRAIN_SIZE);
    int gridZ = floor(playerZ/_TERRAIN_SIZE);
    
    //std::cout << "computed GridX" << gridX << std::endl;
    //std::cout << "computed GridZ" << gridZ << std::endl;
    //
    //std::cout << "_terrainXPlayer: " << _terrainXPlayer << std::endl;
    //std::cout << "_terrainZPlayer: " << _terrainZPlayer << std::endl;

    // check if tile has changed
    if((gridX != _terrainXPlayer) ||(gridZ != _terrainZPlayer)){
        //std::cout <<  "CHANGED TILES!!!!!!!!!!" << std::endl;
        _terrainXPlayer = gridX;
        _terrainZPlayer = gridZ;
        refreshTerrainTiles();
    }

    renderTerrains(camera, deltaTime);
}

void TerrainLoader::customProcess(std::string camera, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    float playerX = _player->getPosition().x();
    float playerZ = _player->getPosition().z();
    // check in which tile the player is
    int gridX = floor(playerX/_TERRAIN_SIZE);
    int gridZ = floor(playerZ/_TERRAIN_SIZE);
    
    // check if tile has changed
    if((gridX != _terrainXPlayer) ||(gridZ != _terrainZPlayer)){
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
    std::string key0 = generateTerrainKey(_terrainXPlayer, _terrainZPlayer);
    newTerrainKeys.push_back(key0);
    auto t = std::make_tuple(_terrainXPlayer, _terrainZPlayer);
    terrainIndexPairs.push_back(t);
    
    // UP
    std::string key1 = generateTerrainKey(_terrainXPlayer, _terrainZPlayer+1);
    newTerrainKeys.push_back(key1);
    t = std::make_tuple(_terrainXPlayer, _terrainZPlayer+1);
    terrainIndexPairs.push_back(t);
    
    // DOWN
    std::string key2 = generateTerrainKey(_terrainXPlayer, _terrainZPlayer-1);
    newTerrainKeys.push_back(key2);
    t = std::make_tuple(_terrainXPlayer, _terrainZPlayer-1);
    terrainIndexPairs.push_back(t);
    
    // LEFT
    std::string key3 = generateTerrainKey(_terrainXPlayer-1, _terrainZPlayer);
    newTerrainKeys.push_back(key3);
    t = std::make_tuple(_terrainXPlayer-1, _terrainZPlayer);
    terrainIndexPairs.push_back(t);
    
    // RIGHT
    std::string key4 = generateTerrainKey(_terrainXPlayer+1, _terrainZPlayer);
    newTerrainKeys.push_back(key4);
    t = std::make_tuple(_terrainXPlayer+1, _terrainZPlayer);
    terrainIndexPairs.push_back(t);
    
    // UPPER-LEFT
    std::string key5 = generateTerrainKey(_terrainXPlayer-1, _terrainZPlayer+1);
    newTerrainKeys.push_back(key5);
    t = std::make_tuple(_terrainXPlayer-1, _terrainZPlayer+1);
    terrainIndexPairs.push_back(t);
    
    // BOTTOM-LEFT
    std::string key6 = generateTerrainKey(_terrainXPlayer-1, _terrainZPlayer-1);
    newTerrainKeys.push_back(key6);
    t = std::make_tuple(_terrainXPlayer-1, _terrainZPlayer-1);
    terrainIndexPairs.push_back(t);
    
    // TOP-RIGHT
    std::string key7 = generateTerrainKey(_terrainXPlayer+1, _terrainZPlayer+1);
    newTerrainKeys.push_back(key7);
    t = std::make_tuple(_terrainXPlayer+1, _terrainZPlayer+1);
    terrainIndexPairs.push_back(t);
    
    // BOTTOM-RIGHT
    std::string key8 = generateTerrainKey(_terrainXPlayer+1, _terrainZPlayer-1);
    newTerrainKeys.push_back(key8);
    t = std::make_tuple(_terrainXPlayer+1, _terrainZPlayer-1);
    terrainIndexPairs.push_back(t);
    
    // compare and rearrange current terrain map and decide which new terrains should be rendered
    for (int i = 0; i<9; i++) {
        if(_terrains.find(newTerrainKeys[i]) == _terrains.end()){
            // DOES NOT exist already (create TerrainPtr and add it)
            std::cout << "have to generate tile: " << newTerrainKeys[i] << std::endl;
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

void TerrainLoader::renderTerrainTilesOnly(std::string camera, const double &deltaTime)
{
    _renderer.getObjects()->getShader("terrain")->setUniform("bloomPass", 1.0);
    TerrainMap::iterator it;
    for (auto const& x: _terrains) {
        x.second->renderTerrain(camera);
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

TerrainPtr TerrainLoader::getSingleTerrain(std::string terrainKey)
{
    return this->_terrains[terrainKey];
}


void TerrainLoader::reloadTerrains()
{
	_seed = 1 + (rand() % static_cast<int>(10000 - 1 + 1));

	for (auto const& x : _terrains) {
		_renderer.getObjects()->removeModel(x.second->getModelName(), true);
	}
	_terrains.clear();

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
