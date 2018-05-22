#include "Entity.h"
#include "Sun.h"
#include "iostream"


Sun::Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
	: Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
	//_sunMaterial = renderer.getObjects()->loadObjMaterial("sun.mtl", "sun", shader);
	_sunProperties = renderer.getObjects()->createProperties("sun");
	MaterialPtr sunMaterial = renderer.getObjects()->loadObjMaterial("sun.mtl", "sun", shader);
	_renderer = renderer;
	//ModelPtr ObjectManager::createSprite(const std::string &name, const std::string &textureFileName, ShaderPtr shader, bool flipT, PropertiesPtr properties)
	//_renderer.getObjects()->createSprite("sun", "sun.png", shader, false ,_sunProperties);
	_renderer.getObjects()->createSprite_o("sun", sunMaterial, NO_OPTION, _sunProperties);
	//TexturePtr sunTexture = _renderer.getObjects()->createTexture("sun_tex", TextureData("sun.png"));
	//_renderer.getObjects()->addModel("sun", generateGeometry());
	
	ShaderPtr flameShader = renderer.getObjects()->loadShaderFile_o("flame", 0, AMBIENT_LIGHTING);				// load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
	MaterialPtr flameMaterial = renderer.getObjects()->loadObjMaterial("flame.mtl", "flame", flameShader);
	// create additional properties for a model
	PropertiesPtr flameProperties = renderer.getObjects()->createProperties("flameProperties");
	// create sprites
	renderer.getObjects()->createSprite_o("flame", flameMaterial, NO_OPTION, flameProperties);

	//bRenderer().getObjects()->getProperties("flameProperties")->setScalar("offset", _randomOffset);
}

ModelPtr Sun::generateGeometry()
{
	_objLoader.addVertex(-1.0f, -1.0f, 0.0f);
	_objLoader.addVertex(-1.0f, 1.0f, 0.0f);
	_objLoader.addVertex(1.0f, 1.0f, 0.0f);
	_objLoader.addVertex(1.0f, -1.0f, 0.0f);

	// Add first face
	IndexData d1, d2, d3;
	d1.vertexIndex = 0;
	d2.vertexIndex = 1;
	d3.vertexIndex = 2;
	_objLoader.addFaceNoTex(d1, d2, d3);

	// Add second face
	IndexData d4, d5, d6;
	d4.vertexIndex = 0;
	d5.vertexIndex = 2;
	d6.vertexIndex = 3;
	_objLoader.addFaceNoTex(d4, d5, d6);

	_objLoader.load();

	 _data = _objLoader.getData();
	 _sunModel = ModelPtr(new Model(_data, _sunMaterial, _sunProperties));

	 return _sunModel;
}

void Sun::render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD)
{
	// draw model instance
	setPosition(vmml::Vector3f(playerPos.x() + 450, 100, playerPos.z()));

	setScale(25.0f);
	setRotY(90.0f);
	vmml::Matrix4f modelMatrix = computeTransformationMatrix();
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//
	//_renderer.getModelRenderer()->queueModelInstance(_renderer.getObjects()->getModel("sun"), "sun_instance", 
	//	modelMatrix, viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false, false, true, GL_SRC_ALPHA, GL_ONE);
	//_renderer.getModelRenderer()->queueModelInstance(_renderer.getObjects()->getModel("flame"), ("flame_instance" + std::to_string(1)), modelMatrix, viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false, false, true, GL_SRC_ALPHA, GL_ONE, (-1.0f - 0.01f*1));
	_renderer.getModelRenderer()->queueModelInstance("sun", "sun_instance", camera, modelMatrix, std::vector<std::string>({}), false, false, true, GL_SRC_ALPHA, GL_ONE, 20000.0f);
}