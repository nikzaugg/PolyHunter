#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "math.h"
#include "noise.h"
#include "iostream"

float ** _heights;
using namespace noise;
module::Perlin perlin;

Terrain::Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
: Entity(modelName, materialFile, materialName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
    std::cout << "TERRAIN WORKS!!!" << std::endl;
    this->_VERTEX_COUNT = 100;
    this->_SIZE = 300;
    this->_amplitude = 70;
    this->_exponent = 4.18;
    this->_maxHeight = 0.0f;
    
    _data = generate();
    ModelPtr terrainModel = ModelPtr(new Model(this->_data, getMaterial(), getProperties()));
    SetModel(terrainModel);
    renderer.getObjects()->addModel("terrain", terrainModel);
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
    float gridSquareSize = _SIZE / (float) sizeHeights;
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

ModelData::GroupMap Terrain::generate()
{
    ProceduralOBJLoader objLoader;
    
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
    
    int counter = 0;
    for (int i = 0; i < _VERTEX_COUNT - 1; i++)
    {
        
        for (int j = 0; j < _VERTEX_COUNT - 1; j++)
        {
            float uPos = (float)j / ((float)_VERTEX_COUNT - 1);
            float vPos = (float)i / ((float)_VERTEX_COUNT - 1);
            uPos = 1 - uPos;
            vPos = 1 - vPos;
            objLoader.addTexCoords(uPos, vPos);
            
            float xTopLeft = ((float)j / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            float zTopLeft = ((float)i / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            
            float xTopRight = ((float)(j+1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            float zTopRight = ((float)i / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            
            float xBottomLeft = ((float)j / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            float zBottomLeft = ((float)(i+1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            
            float xBottomRight = ((float)(j + 1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            float zBottomRight = ((float)(i + 1) / ((float)_VERTEX_COUNT - 1)) * _SIZE;
            
            objLoader.addVertex(xTopLeft, _heights[i][j], zTopLeft);
            objLoader.addVertex(xTopRight, _heights[i][j+1], zTopRight);
            objLoader.addVertex(xBottomLeft, _heights[i+1][j], zBottomLeft);
            
            
            IndexData d1, d2, d3;
            d1.vertexIndex = counter++;
            d2.vertexIndex = counter++;
            d3.vertexIndex = counter++;
            
            objLoader.addFace(d1, d2, d3);
            
            objLoader.addVertex(xBottomLeft, _heights[i+1][j], zBottomLeft);
            objLoader.addVertex(xTopRight, _heights[i][j+1], zTopRight);
            objLoader.addVertex(xBottomRight, _heights[i+1][j+1], zBottomRight);
            
            IndexData d4, d5, d6;
            d4.vertexIndex = counter++;
            d5.vertexIndex = counter++;
            d6.vertexIndex = counter++;
            
            objLoader.addFace(d4, d5, d6);
        }
    }
    
    objLoader.load();
    
    ModelData::GroupMap data = objLoader.getData();
    return data;
}

void Terrain::process(std::string cameraName, const double &deltaTime)
{
    render(cameraName);
}

void Terrain::render(std::string camera)
{
    getShader()->setUniform("amplitude", _amplitude);
    getShader()->setUniform("heightPercent", _maxHeight / 100);
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    std::cout << computeTransformationMatrix() << std::endl;
    // draw model
    renderer().getModelRenderer()->drawModel("terrain", camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}
