#include "Entity.h"
#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "math.h"
#include "noise.h"
#include "iostream"

using namespace noise;
module::Perlin perlin;

Terrain::Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
: Entity(modelName, materialFile, materialName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
    std::cout << "TERRAIN WORKS!!!" << std::endl;
    _amplitude = 40;
    _exponent = 4.18;
    _maxHeight = 0.0f;
    _TERRAIN_SIZE = 500;
    _VERTEX_COUNT = 97;
    
    _data = generate();
    
    ModelPtr terrainModel = ModelPtr(new Model(_data, getMaterial(), getProperties()));
    SetModel(terrainModel);
    renderer.getObjects()->addModel("terrain", terrainModel);
    _objLoader = ProceduralOBJLoader();
}

double Terrain::noise(double nx, double ny) {
    // Rescale from -1.0:+1.0 to 0.0:1.0
    return perlin.GetValue(nx, ny, 0) / 2.0 + 0.5;
}

float Terrain::barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos) {
    float det = (p2.z() - p3.z()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.z() - p3.z());
    float l1 = ((p2.z() - p3.z()) * (pos.x() - p3.x()) + (p3.x() - p2.x()) * (pos.y() - p3.z())) / det;
    float l2 = ((p3.z() - p1.z()) * (pos.x() - p3.x()) + (p1.x() - p3.x()) * (pos.y() - p3.z())) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y() + l2 * p2.y() + l3 * p3.y();
}

float Terrain::getHeightOfTerrain(float worldX, float worldZ){
    float terrainX = worldX - getPosition().x();
    float terrainZ = worldZ - getPosition().z();
    int sizeHeights = (_VERTEX_COUNT) - 1;
    //    std::cout << "size of heights: " << sizeHeights << std::endl;
    float gridSquareSize = _TERRAIN_SIZE / (float) sizeHeights;
    int gridX = floor(terrainX / gridSquareSize);
    int gridZ = floor(terrainZ / gridSquareSize);
    //    std::cout << "gridX: " << gridX << std::endl;
    //    std::cout << "gridZ: " << gridZ << std::endl;
    if (gridX >= sizeHeights || gridZ >= sizeHeights || gridX < 0 || gridZ < 0) {
        return 0.0;
    }
    float xCoord = remainderf(terrainX, gridSquareSize) / gridSquareSize;
    float zCoord = remainderf(terrainZ, gridSquareSize) / gridSquareSize;
    float result;
    if (xCoord <= (1-zCoord)) {
        result = barryCentric(
                              vmml::Vector3f(0, _heights[gridX][gridZ], 0),
                              vmml::Vector3f(1, _heights[gridX + 1][gridZ], 0),
                              vmml::Vector3f(0, _heights[gridX][gridZ + 1], 1),
                              vmml::Vector2f(xCoord, zCoord)
                              );
    } else {
        result = barryCentric(
                              vmml::Vector3f(1, _heights[gridX + 1][gridZ], 0),
                              vmml::Vector3f(1,_heights[gridX + 1][gridZ + 1], 1),
                              vmml::Vector3f(0,_heights[gridX][gridZ + 1], 1),
                              vmml::Vector2f(xCoord, zCoord)
                              );
    }
    return result;
}

float ** Terrain::getTerrainHeights()
{
    return _heights;
}

ModelData::GroupMap Terrain::generate()
{
    generateHeights();
    generateVertices();
    generateIdices();
    
    _objLoader.load();
    
    ModelData::GroupMap data = _objLoader.getData();
    return data;
}

void Terrain::generateHeights()
{
    _heights = new float*[_VERTEX_COUNT];
    // noise generation
    for (int  i = 0; i < _VERTEX_COUNT; i++)
    {
        _heights[i] = new float[_VERTEX_COUNT];
        for (int j = 0; j < _VERTEX_COUNT; j++)
        {
            float nx = ((float)j / ((float)_VERTEX_COUNT)) - 0.5;
            float nz = ((float)i / ((float)_VERTEX_COUNT)) - 0.5;
            
            perlin.SetSeed(549);
            float height = 1 * noise(1 * nx, 1 * nz)
            + 0.5 * noise(2 * nx, 2 * nz)
            + 0.25 * noise(4 * nx, 4 * nz);
            
            height = pow(height, _exponent);
            
            _heights[i][j] = height * _amplitude;
            
            if (_maxHeight < _heights[i][j])
            {
                _maxHeight = _heights[i][j];
            }
        }
    }
}

void Terrain::generateVertices()
{
    for (int i = 0; i < _VERTEX_COUNT-1; i++)
    {
        for (int j = 0; j < _VERTEX_COUNT-1; j++)
        {
            float xTopLeft = ((float)i / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            float zTopLeft = ((float)j / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            
            // std::cout << "TopLeft: "<< xTopLeft << " | " << zTopLeft << std::endl;
            
            float xTopRight = ((float)(i) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            float zTopRight = ((float)(j+1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            
            // std::cout << "TopRight: "<< xTopRight << " | " << zTopRight << std::endl;
            
            float xBottomLeft = ((float)(i+1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            float zBottomLeft = ((float)(j) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            
            // std::cout << "BottomLeft: "<< xBottomLeft << " | " << zBottomLeft << std::endl;
            
            float xBottomRight = ((float)(i + 1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            float zBottomRight = ((float)(j + 1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            
            // std::cout << "BottomRight: "<< xBottomRight << " | " << zBottomRight << std::endl;
            // std::cout << " ---------------------" << std::endl;
            
            _objLoader.addVertex(xTopLeft, _heights[j][i], zTopLeft);
            _objLoader.addVertex(xBottomLeft, _heights[j][i+1], zBottomLeft);
            _objLoader.addVertex(xTopRight, _heights[j+1][i], zTopRight);
            
            _objLoader.addVertex(xTopRight, _heights[j+1][i], zTopRight);
            _objLoader.addVertex(xBottomLeft, _heights[j][i+1], zBottomLeft);
            _objLoader.addVertex(xBottomRight, _heights[j+1][i+1], zBottomRight);
        }
    }
}

void Terrain::generateIdices()
{
    int counter = 0;
    
    for (int i = 0; i<_VERTEX_COUNT-1; i++) {
        for (int j = 0; j < _VERTEX_COUNT-1 ; j++) {
            
            
            IndexData d1, d2, d3;
            d1.vertexIndex = counter++;
            d2.vertexIndex = counter++;
            d3.vertexIndex = counter++;
            
            _objLoader.addFaceNoTex(d1, d2, d3);
            
            IndexData d4, d5, d6;
            d4.vertexIndex = counter++;
            d5.vertexIndex = counter++;
            d6.vertexIndex = counter++;
            
            _objLoader.addFaceNoTex(d4, d5, d6);
        }
    }
    
    std::cout << "Nr of Indices created: "<< counter << std::endl;

}

void Terrain::process(std::string cameraName, const double &deltaTime)
{
    render(cameraName);
}

void Terrain::render(std::string camera)
{
    getShader()->setUniform("amplitude", _amplitude);
    getShader()->setUniform("heightPercent", _maxHeight / 100);
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    // draw model
    renderer().getModelRenderer()->drawModel("terrain", camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}
