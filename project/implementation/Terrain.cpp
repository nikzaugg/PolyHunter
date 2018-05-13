#include "Entity.h"
#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "Tree.h"
#include "math.h"
#include "iostream"
#include "noise.h"

Terrain::Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, int gridX, int gridZ ,int terrain_size, int vertex_count, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
: Entity(modelName, materialFile, materialName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
    std::cout << "TERRAIN WORKS!!!" << std::endl;
    _gridX = gridX;
    _gridZ = gridZ;
    _amplitude = 30;
    _exponent = 4.18;
    _TERRAIN_SIZE = terrain_size;
    _VERTEX_COUNT = vertex_count;
    
    this->_offsetX = gridX * _TERRAIN_SIZE;
    this->_offsetZ = gridZ * _TERRAIN_SIZE;
    
    _data = generate();
    
    ModelPtr terrainModel = ModelPtr(new Model(_data, getMaterial(), getProperties()));
    SetModel(terrainModel);
    renderer.getObjects()->addModel(getModelName(), terrainModel);
    _objLoader = ProceduralOBJLoader();
	placeTrees();
}

float Terrain::barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos) {
    float det = (p2.z() - p3.z()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.z() - p3.z());
    float l1 = ((p2.z() - p3.z()) * (pos.x() - p3.x()) + (p3.x() - p2.x()) * (pos.y() - p3.z())) / det;
    float l2 = ((p3.z() - p1.z()) * (pos.x() - p3.x()) + (p1.x() - p3.x()) * (pos.y() - p3.z())) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y() + l2 * p2.y() + l3 * p3.y();
}

ModelData::GroupMap Terrain::generate()
{
    generateVertices();
    generateIdices();
    
    _objLoader.load();
    
    ModelData::GroupMap data = _objLoader.getData();
    return data;
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
            
            //std::cout << getHeightFromNoise(xTopLeft, zTopLeft) << std::endl;

            // flip z-coords if windows-device
			if (!Input::isTouchDevice()) {
				zTopLeft *= -1;
				zBottomLeft *= -1;
				zTopRight *= -1;
				zBottomRight *= -1;
			}
            
            _objLoader.addVertex(xTopLeft, getHeightFromNoise(getNoiseInput(xTopLeft), getNoiseInput(zTopLeft)), zTopLeft);
            _objLoader.addVertex(xBottomLeft, getHeightFromNoise(getNoiseInput(xBottomLeft),getNoiseInput(zBottomLeft)), zBottomLeft);
            _objLoader.addVertex(xTopRight, getHeightFromNoise(getNoiseInput(xTopRight), getNoiseInput(zTopRight)), zTopRight);
            
            _objLoader.addVertex(xTopRight, getHeightFromNoise(getNoiseInput(xTopRight), getNoiseInput(zTopRight)), zTopRight);
            _objLoader.addVertex(xBottomLeft, getHeightFromNoise(getNoiseInput(xBottomLeft), getNoiseInput(zBottomLeft)), zBottomLeft);
            _objLoader.addVertex(xBottomRight, getHeightFromNoise(getNoiseInput(xBottomRight), getNoiseInput(zBottomRight)), zBottomRight);
        }
    }
}

double Terrain::getNoiseInput(float coord)
{
    return coord / (float)_TERRAIN_SIZE;
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
	renderTrees(camera);
}

void Terrain::placeTrees()
{
	ShaderPtr basicShader = renderer().getObjects()->loadShaderFile("basic", 1, false, true, true, true, false);
	PropertiesPtr treeProperties = renderer().getObjects()->createProperties("treeProperties");

	noise::module::RidgedMulti ridgedMulti;

	ridgedMulti.SetSeed(100);


	for (int i = 0; i < _VERTEX_COUNT; i++) 
	{
		for (int j = 0; j < _VERTEX_COUNT; j++)
		{
			float treeHeight = getHeightFromNoise(getNoiseInput((float)i), getNoiseInput((float)j));

			// Rescale from -1.0:+1.0 to 0.0:1.0
			float value = ridgedMulti.GetValue((float)i, treeHeight, (float)j);
			std::cout << value << std::endl;
			if (value > 1.4f)
			{
				_trees.insert(
					TreeMap::value_type(
						getModelName() + std::to_string(i),
						TreePtr(new Tree(getModelName() + std::to_string(i), "tree.obj", "tree", "treeProperties", basicShader, renderer(), vmml::Vector3f((float)i, treeHeight, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f))
					)
				);
			}

		}
		
	}
}

Terrain::TreeMap Terrain::getTreeMap()
{
	return _trees;
}

void Terrain::renderTrees(std::string camera)
{
	TreeMap::iterator it;
	for (auto const& x : _trees) {
		x.second->render(camera);
	}
}
