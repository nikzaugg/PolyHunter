#ifndef B_SUN_H
#define B_SUN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"

class Sun : public Entity
{

public:
	Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);

	ModelPtr generateGeometry();

	void render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD);

	void render(std::string camera, vmml::Vector3f playerPos);

private:
	ProceduralOBJLoader _objLoader;
	ModelData::GroupMap _data;
	ModelPtr _sunModel;
	Renderer _renderer;
	MaterialPtr _sunMaterial;
	PropertiesPtr _sunProperties;

};
typedef std::shared_ptr< Sun >  SunPtr;
#endif /* defined(B_SUN_H) */