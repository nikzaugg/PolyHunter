#include "Entity.h"
#include "Terrain.h"
#include "ProceduralOBJLoader.h"
#include "PerlinNoise.h"
#include "PerlinNoise2D.h"
#include "Tree.h"
#include "math.h"
#include "iostream"
#include "noise.h"
#include <ctime>

Terrain::CrystalMap Terrain::_collectedCrystals;

int Terrain::seed = Terrain::getRandomSeed();
int Terrain::TERRAIN_SIZE = 400;
int Terrain::VERTEX_COUNT = 30;
float Terrain::AMPLITUDE = 300;

Terrain::Terrain(std::string modelName, std::string materialFile, std::string materialName, std::string propName, ShaderPtr shader, Renderer & renderer, int gridX, int gridZ ,int terrain_size, int vertex_count, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
: Entity(modelName, materialFile, materialName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
    _gridX = gridX;
    _gridZ = gridZ;
    Terrain::TERRAIN_SIZE = terrain_size;
    Terrain::VERTEX_COUNT = vertex_count;
    _amplitude = 70;

    this->_offsetX = gridX * Terrain::TERRAIN_SIZE;
    this->_offsetZ = gridZ * Terrain::TERRAIN_SIZE;
    
    _data = generateTerrain();
    ModelPtr terrainModel = ModelPtr(new Model(_data, getMaterial(), getProperties()));
    SetModel(terrainModel);
    renderer.getObjects()->addModel(getModelName(), terrainModel);
}

ModelData::GroupMap Terrain::generateTerrain()
{
    generateTerrainGeometry();

    _objLoader.load();

    ModelData::GroupMap data = _objLoader.getData();
    return data;
}

void Terrain::generateTerrainGeometry()
{
	srand(NULL);
    int counter = 0;
    for (int i = 0; i < Terrain::VERTEX_COUNT -1; i++)
    {
        for (int j = 0; j < Terrain::VERTEX_COUNT-1; j++)
        {
            /***************************
             CALCULATE VERTEX POSITIONS
             **************************/
            float xTopLeft = ((float)i / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            xTopLeft += _offsetX;
            float zTopLeft = ((float)j / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            zTopLeft += _offsetZ;

            // std::cout << "TopLeft: "<< xTopLeft << " | " << zTopLeft << std::endl;

            float xTopRight = ((float)(i) / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            xTopRight += _offsetX;
            float zTopRight = ((float)(j+1) / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            zTopRight += _offsetZ;

            // std::cout << "TopRight: "<< xTopRight << " | " << zTopRight << std::endl;

            float xBottomLeft = ((float)(i+1) / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            xBottomLeft += _offsetX;
            float zBottomLeft = ((float)(j) / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            zBottomLeft += _offsetZ;

            // std::cout << "BottomLeft: "<< xBottomLeft << " | " << zBottomLeft << std::endl;

            float xBottomRight = ((float)(i + 1) / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            xBottomRight += _offsetX;
            float zBottomRight = ((float)(j + 1) / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
            zBottomRight += _offsetZ;

            // std::cout << "BottomRight: "<< xBottomRight << " | " << zBottomRight << std::endl;
            // std::cout << " ---------------------" << std::endl;

            //std::cout << getHeightFromNoise(xTopLeft, zTopLeft) << std::endl;

            /*********************
             ADD CREATED VERTICES
             ********************/
            _objLoader.addVertex(xTopLeft, getHeightFromNoise(getNoiseInput(xTopLeft), getNoiseInput(zTopLeft)), zTopLeft);
            _objLoader.addVertex(xBottomLeft, getHeightFromNoise(getNoiseInput(xBottomLeft),getNoiseInput(zBottomLeft)), zBottomLeft);
            _objLoader.addVertex(xTopRight, getHeightFromNoise(getNoiseInput(xTopRight), getNoiseInput(zTopRight)), zTopRight);

            _objLoader.addVertex(xTopRight, getHeightFromNoise(getNoiseInput(xTopRight), getNoiseInput(zTopRight)), zTopRight);
            _objLoader.addVertex(xBottomLeft, getHeightFromNoise(getNoiseInput(xBottomLeft), getNoiseInput(zBottomLeft)), zBottomLeft);
            _objLoader.addVertex(xBottomRight, getHeightFromNoise(getNoiseInput(xBottomRight), getNoiseInput(zBottomRight)), zBottomRight);
            
            /*********************
             CREATE & ADD INDICES
             ********************/
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
            
            /******************
             CREATE TREES
             *****************/
            placeTree(i, j);
            placeCrystal(i, j);
			placeRocks(i, j);
        }
    }
}

void Terrain::placeTree(int i, int j)
{
    noise::module::RidgedMulti ridgedMulti;
    ridgedMulti.SetSeed(100);
    
    // Rescale from -1.0:+1.0 to 0.0:1.0
    float xPos = ((float)i / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
    xPos += _offsetX;
    
    float zPos = ((float)j / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
    zPos += _offsetZ;
    
    float treeHeight;
    treeHeight = Terrain::getHeightFromNoise(getNoiseInput(xPos), getNoiseInput(zPos));
	
	float normHeight = treeHeight / Terrain::AMPLITUDE;

    float value = ridgedMulti.GetValue(xPos, treeHeight, zPos);
    if (value > 1.0f)
    {
		TerrainObject tree;
	//if (normHeight > 0.9f)
	//	{
			tree = {
				vmml::Vector3f(xPos, treeHeight, zPos),
				5.0f,
				"ThinTree"
			};
	//	}
	//else if (normHeight > 0.8f) {
		//tree = {
		//	vmml::Vector3f(xPos, treeHeight, zPos),
		//	1.0f,
		//	"tree"
		//};
	//}

		_trees.insert(
			TreeMap::value_type(tree.type + std::to_string(i) + std::to_string(j), tree)
		);
        
    }
}

void Terrain::placeCrystal(int i, int j)
{
    noise::module::RidgedMulti ridgedMulti;
    ridgedMulti.SetSeed(50);
    
    // Rescale from -1.0:+1.0 to 0.0:1.0
    float xPos = ((float)i / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
    xPos += _offsetX;
    
    float zPos = ((float)j / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
    zPos += _offsetZ;
    
    float crystalHeight;
    crystalHeight = getHeightFromNoise(getNoiseInput(xPos), getNoiseInput(zPos));

	float normHeight = crystalHeight / Terrain::AMPLITUDE;

    float value = ridgedMulti.GetValue(xPos, crystalHeight, zPos);
    if (value > 1.0f && normHeight > 0.5f)
    {
        std::string crystalName = getModelName() + std::to_string(i);
        if(Terrain::_collectedCrystals.find(crystalName) != Terrain::_collectedCrystals.end()){
            std::cout << "crystal already collected!" << std::endl;
        }
            CrystalPtr crystal = CrystalPtr(new Crystal(crystalName, "crystal.obj", "crystal", "crystalProperties", renderer().getObjects()->loadShaderFile("basic", 1, false, true, true, true, false), renderer(), vmml::Vector3f(xPos, crystalHeight, zPos), 0.0f, 0.0f, 0.0f, 2.0f));
            
            crystal->setYPosition(crystalHeight);
            crystal->setYPosition(crystalHeight);

            _crystals.insert(
                             CrystalMap::value_type(crystalName, crystal)
                             );
            _treeCount++;
        
    }
}

void Terrain::placeRocks(int i, int j)
{
	noise::module::RidgedMulti ridgedMulti;
	ridgedMulti.SetSeed(100);

	// Rescale from -1.0:+1.0 to 0.0:1.0
	float xPos = ((float)i / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
	xPos += _offsetX;

	float zPos = ((float)j / ((float)Terrain::VERTEX_COUNT - 1)) * Terrain::TERRAIN_SIZE;
	zPos += _offsetZ;

	float height = getHeightFromNoise(getNoiseInput(xPos), getNoiseInput(zPos));

	float value = ridgedMulti.GetValue(xPos, height, zPos);
	if (0.8f < value && value < 0.9f)
	{
		TerrainObject rock = {
			vmml::Vector3f(xPos, height, zPos),
			5.0f,
			"Rock" + std::to_string(1 + (rand() % static_cast<int>(4 - 1 + 1)))
		};

		_rocks.insert(RockMap::value_type("Rock" + std::to_string(i) + std::to_string(j), rock));
	}
}

void Terrain::process(std::string cameraName, const double &deltaTime)
{
    processTrees(cameraName);
    processCrystals(cameraName);
	processRocks(cameraName);
    renderTerrain(cameraName);
}

void Terrain::customProcess(std::string cameraName, const double &deltaTime, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    customProcessTrees(cameraName, view, proj);
    customRenderTerrain(cameraName, view, proj);
}

void Terrain::processTrees(std::string camera)
{
	vmml::Matrix4f modelMatrix;
	for (auto const& tree : _trees)
	{
		modelMatrix =
			vmml::create_translation(tree.second.position) *
			vmml::create_scaling(vmml::Vector3f(tree.second.scale));
		renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
		renderer().getModelRenderer()->queueModelInstance(tree.second.type, tree.first, camera, modelMatrix, std::vector<std::string>({ "sun", "torch" }), true, true);
	}
}

void Terrain::processCrystals(std::string camera)
{
    CrystalMap::iterator it;
    for (auto const& x : _crystals) {
        if (Terrain::_collectedCrystals.find(x.first) == Terrain::_collectedCrystals.end()) {
            x.second->render(camera);
        }
    }
}

void Terrain::processRocks(std::string camera)
{
	vmml::Matrix4f modelMatrix;
	for (auto const& rock : _rocks)
	{
		modelMatrix =
			vmml::create_translation(rock.second.position) *
			vmml::create_scaling(vmml::Vector3f(rock.second.scale));
		renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
		renderer().getModelRenderer()->queueModelInstance(rock.second.type, rock.first, camera, modelMatrix, std::vector<std::string>({ "sun", "torch" }), true, true);
	}
}

void Terrain::drawCrystals(std::string camera)
{
    CrystalMap::iterator it;
    for (auto const& x : _crystals) {
        if (Terrain::_collectedCrystals.find(x.first) == Terrain::_collectedCrystals.end()) {
            x.second->draw(camera);
        }
    }
}

void Terrain::customProcessTrees(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj)
{
  /*  TreeMap::iterator it;
    for (auto const& x : _trees) {
        x.second->customRender(camera, view, proj);
    }*/
}

void Terrain::renderTerrain(std::string camera)
{
    getShader()->setUniform("amplitude", _amplitude);
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    // draw model
    renderer().getModelRenderer()->queueModelInstance(getModelName(), "terrain", camera, computeTransformationMatrix(), std::vector<std::string>({ "sun", "torch" }), true, true);

}

void Terrain::customRenderTerrain(std::string camera, vmml::Matrix4f view, vmml::Matrix4f proj)
{
    getShader()->setUniform("amplitude", _amplitude);
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    // draw model
    // renderer().getModelRenderer()->drawModel(renderer().getObjects()->getModel(getModelName()), computeTransformationMatrix(), view, proj, std::vector<std::string>({}), false);
}

Terrain::TreeMap Terrain::getTreeMap()
{
	return _trees;
}

bool Terrain::checkCollisionWithEntities(vmml::Vector3f playerPos)
{
    vmml::Vector3f crystalPos;
    bool collision = false;
    float playerCameraGroundOffset = 10.0;
    
    for(CrystalMap::iterator it = _crystals.begin(); it != _crystals.end(); it++)
    {
        crystalPos = it->second->getPosition();
        float distance = sqrtf(pow(playerPos.x() - crystalPos.x(), 2.0) + pow(playerPos.y() - (crystalPos.y()+playerCameraGroundOffset), 2.0) + pow(playerPos.z() - crystalPos.z(), 2.0));
        
        // if close enough to crystal
        if(distance <= 15.0){
            // calculate angle between torchlight direction and crystal-position
            vmml::Vector3f camDir = renderer().getObjects()->getCamera("camera")->getForward();
            vmml::Vector3f torchPos = renderer().getObjects()->getLight("torch")->getPosition();
            vmml::Vector3f crystalPos = it->second->getPosition();
            camDir = vmml::normalize(camDir);
            vmml::Vector3f modelToTorch = vmml::normalize(torchPos - crystalPos);
            float angle = vmml::dot(modelToTorch, -camDir);
            
            float scale = it->second->getScale();
            // if small enough, add it to collected crystals
            if(scale < 1.0){
                Terrain::_collectedCrystals.insert(CrystalMap::value_type(it->first , it->second));
            } else if (scale > 0.25 && angle > 0.985) {
                // if angle is small enough, slowly shrink crystal
                it->second->setScale(scale * 0.98);
            }
            collision = true;
        }
    }

    return collision;
}

int Terrain::getNrOfCrystalsCollected()
{
    return Terrain::_collectedCrystals.size();
}

float Terrain::barryCentric(vmml::Vector3f p1, vmml::Vector3f p2, vmml::Vector3f p3, vmml::Vector2f pos) {
    float det = (p2.z() - p3.z()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.z() - p3.z());
    float l1 = ((p2.z() - p3.z()) * (pos.x() - p3.x()) + (p3.x() - p2.x()) * (pos.y() - p3.z())) / det;
    float l2 = ((p3.z() - p1.z()) * (pos.x() - p3.x()) + (p1.x() - p3.x()) * (pos.y() - p3.z())) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.y() + l2 * p2.y() + l3 * p3.y();
}

double Terrain::noise(double nx, double nz)
{
	noise::module::Perlin perlin;
	perlin.SetSeed(Terrain::seed);
	perlin.SetOctaveCount(4);
	perlin.SetFrequency(1);
	perlin.SetLacunarity(2.5);

	// Rescale from -1.0:+1.0 to 0.0:1.0
	double value = perlin.GetValue(nx, nz, 0.0) / 2.0 + 0.5;

	// Prevent NaN error by not allowing values below 0
	value = value < 0.0 ? 0.0 : value;

	return value;
}

float Terrain::getHeightFromNoise(double nx, double nz)
{
	// Rescale from -1.0:+1.0 to 0.0:1.0
	float res = Terrain::noise(nx, nz);
	res = pow(res, 1.27);
	res *= Terrain::AMPLITUDE;
	return res;
}

double Terrain::getNoiseInput(float coord)
{
	return coord / (float)(Terrain::TERRAIN_SIZE * 3);
}

int Terrain::getRandomSeed()
{
	srand(time(NULL));
	return 10 + (rand() % static_cast<int>(10000 - 10 + 1));
}
