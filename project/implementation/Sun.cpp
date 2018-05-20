#include "Entity.h"
#include "Sun.h"
#include "iostream"


Sun::Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale)
	: Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
{
	//_sunMaterial = renderer.getObjects()->loadObjMaterial("sun.mtl", "sun", shader);
	_sunProperties = renderer.getObjects()->createProperties("sun");
	_renderer = renderer;
	//ModelPtr ObjectManager::createSprite(const std::string &name, const std::string &textureFileName, ShaderPtr shader, bool flipT, PropertiesPtr properties)
	_renderer.getObjects()->createSprite("sun", "sun.png", shader, false ,_sunProperties);
	//TexturePtr sunTexture = _renderer.getObjects()->createTexture("sun_tex", TextureData("sun.png"));
	//_renderer.getObjects()->addModel("sun", generateGeometry());
	
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

void Sun::render(std::string camera, vmml::Vector3f playerPos)
{
	// draw model instance
	setPosition(vmml::Vector3f(0.0f, 100.0f, 0.0f));

	setScale(25.0f);
	setRotY(90.0f);
	vmml::Matrix4f modelMatrix = computeTransformationMatrix();
	ShaderPtr shader = _renderer.getObjects()->getShader("sun");
	_renderer.getModelRenderer()->queueModelInstance("sun", "sun_instance", camera, modelMatrix, std::vector<std::string>({ "sun" }), true, true);
}