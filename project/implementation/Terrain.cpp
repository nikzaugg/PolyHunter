#include "Entity.h"
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
    _amplitude = 70;
    _exponent = 4.18;
    _maxHeight = 0.0f;
    _TERRAIN_SIZE = 300;
    _VERTEX_COUNT = 97;
    
    _data = generate();
    
    ModelPtr terrainModel = ModelPtr(new Model(_data, getMaterial(), getProperties()));
    SetModel(terrainModel);
    renderer.getObjects()->addModel("terrain", terrainModel);
    _objLoader = ProceduralOBJLoader();
}

Terrain::Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, int gridX, int gridZ ,vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
: Entity(modelName, materialFile, materialName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
    std::cout << "TERRAIN WORKS!!!" << std::endl;
    _gridX = gridX;
    _gridZ = gridZ;
    _amplitude = 70;
    _exponent = 4.18;
    _maxHeight = 0.0f;
    _TERRAIN_SIZE = 100;
    _VERTEX_COUNT = 30;
    
    this->_offsetX = gridX * _TERRAIN_SIZE;
    this->_offsetZ = gridZ * _TERRAIN_SIZE;
    
    _data = generate();
    
    ModelPtr terrainModel = ModelPtr(new Model(_data, getMaterial(), getProperties()));
    SetModel(terrainModel);
    renderer.getObjects()->addModel(getModelName(), terrainModel);
    _objLoader = ProceduralOBJLoader();
}

double Terrain::noise(double nx, double nz) {
    // Rescale from -1.0:+1.0 to 0.0:1.0
    double inputX, inputZ;
    inputX = nx / (float)_TERRAIN_SIZE;
    inputZ = nz / (float)_TERRAIN_SIZE;
    float res = perlin.GetValue(inputX, inputZ, 0.0) / 2.0 + 0.5;

	//std::cout << res << std::endl;
    //res = pow(res, _exponent);
    res *= _amplitude;
    return res;
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
            perlin.SetSeed(549);
        
            _heights[i][j] = 0.0;
            
            if (_maxHeight < _heights[i][j])
            {
                _maxHeight = _heights[i][j];
            }
        }
    }
}

void Terrain::generateVertices()
{
    // 0 1 2 3
    for (int i = 0; i < _VERTEX_COUNT-1; i++)
    {
        // 0 1 2 3
        for (int j = 0; j < _VERTEX_COUNT-1; j++)
        {
            float xTopLeft = ((float)i / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            xTopLeft += _offsetX;
            float zTopLeft = ((float)j / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            zTopLeft += _offsetZ;
            
            // std::cout << "TopLeft: "<< xTopLeft << " | " << zTopLeft << std::endl;
            
            float xTopRight = ((float)(i) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            xTopRight += _offsetX;
            float zTopRight = ((float)(j+1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            zTopRight += _offsetZ;
            
            // std::cout << "TopRight: "<< xTopRight << " | " << zTopRight << std::endl;
            
            float xBottomLeft = ((float)(i+1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            xBottomLeft += _offsetX;
            float zBottomLeft = ((float)(j) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            zBottomLeft += _offsetZ;
            
            // std::cout << "BottomLeft: "<< xBottomLeft << " | " << zBottomLeft << std::endl;
            
            float xBottomRight = ((float)(i + 1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            xBottomRight += _offsetX;
            float zBottomRight = ((float)(j + 1) / ((float)_VERTEX_COUNT - 1)) * _TERRAIN_SIZE;
            zBottomRight += _offsetZ;
            
            // std::cout << "BottomRight: "<< xBottomRight << " | " << zBottomRight << std::endl;
            // std::cout << " ---------------------" << std::endl;
            perlin.SetSeed(549);
            
            //std::cout << noise(xTopLeft, zTopLeft) << std::endl;

			if (!Input::isTouchDevice()) {
				zTopLeft *= -1;
				zBottomLeft *= -1;
				zTopRight *= -1;
				zBottomRight *= -1;
			}
            
            _objLoader.addVertex(xTopLeft, noise(xTopLeft, zTopLeft), zTopLeft);
            _objLoader.addVertex(xBottomLeft, noise(xBottomLeft, zBottomLeft), zBottomLeft);
            _objLoader.addVertex(xTopRight, noise(xTopRight, zTopRight), zTopRight);
            
            _objLoader.addVertex(xTopRight, noise(xTopRight, zTopRight), zTopRight);
            _objLoader.addVertex(xBottomLeft, noise(xBottomLeft, zBottomLeft), zBottomLeft);
            _objLoader.addVertex(xBottomRight, noise(xBottomRight, zBottomRight), zBottomRight);
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
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}
