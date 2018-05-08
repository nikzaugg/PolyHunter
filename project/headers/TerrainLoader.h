#ifndef B_TERRAINLOADER_H
#define B_TERRAINLOADER_H

#include "bRenderer.h"
#include "Terrain.h"

class TerrainLoader {
    
public:
    TerrainLoader(Renderer & renderer, ShaderPtr shader);
    void renderTerrains(std::string camera);
    
private:
    TerrainPtr generateTerrain(int gridX, int gridZ);
    
    typedef std::unordered_map< std::string, TerrainPtr > TerrainMap;
    
    TerrainMap _terrains;
    ShaderPtr _shader;
    Renderer _renderer;
    int _nrOfTerrains;
    vmml::Vector3f _centerPoint;
    
};
typedef std::shared_ptr< TerrainLoader >  TerrainLoaderPtr;
#endif /* defined(B_TERRAINLOADER_H) */

