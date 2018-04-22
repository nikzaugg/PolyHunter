#include "Skybox.h"
#include "ProceduralOBJLoader.h"
#include "iostream"


Skybox::Skybox(MaterialPtr material, PropertiesPtr properties)
{
	_material = material;
	_properties = properties;

	std::cout << "Skybox loaded!!!" << std::endl;
}

ModelPtr Skybox::generate()
{
	ProceduralOBJLoader objLoader;

	TextureData left = TextureData("left.png");
	TextureData right = TextureData("right.png");
	TextureData bottom = TextureData("bottom.png");
	TextureData top = TextureData("top.png");
	TextureData front = TextureData("front.png");
	TextureData back = TextureData("back.png");

	CubeMap skyBoxCubeMap = CubeMap(std::vector<TextureData>{left, right, bottom, top, front, back});

	objLoader.addVertex(1.0, -1.0, -1.0);
	objLoader.addVertex(1.0, -1.0, 1.0);
	objLoader.addVertex(-1.0, -1.0, 1.0);
	objLoader.addVertex(-1.0, -1.0, -1.0);
	objLoader.addVertex(1.0, 1.0, -1.0);
	objLoader.addVertex(1.0, 1.0, 1.0);
	objLoader.addVertex(-1.0, 1.0,  1.0);
	objLoader.addVertex(-1.0, 1.0, -1.0);

	// objLoader.addTexCoords(uPos, vPos);
	// objLoader.addVertex(xPos, _heights[i][j], zPos);

	IndexData d1, d2, d3;
	// objLoader.addFace(d1, d2, d3);

	objLoader.load();

	ModelData::GroupMap data = objLoader.getData();
	ModelPtr terrainModel = ModelPtr(new Model(data, _material, _properties));

	return terrainModel;
}
