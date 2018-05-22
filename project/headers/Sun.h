#ifndef B_SUN_H
#define B_SUN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"

class Sun : public Entity
{

public:
	Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);

	void setIntensity(float intensity);

	void setPosition(vmml::Vector3f position);

	float getVertexPos();

	void render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD);

private:
	ProceduralOBJLoader _objLoader;
	ModelData::GroupMap _data;
	ModelPtr _sunModel;
	Renderer _renderer;
	MaterialPtr _sunMaterial;
	PropertiesPtr _sunProperties;
	ModelPtr _sunFragments;

	ModelData::GroupMap Sun::createFragments();

	void renderFragments(std::string camera, vmml::Vector3f pos);

	void renderFragments(std::string camera);

};
typedef std::shared_ptr< Sun >  SunPtr;
#endif /* defined(B_SUN_H) */