#ifndef B_SUN_H
#define B_SUN_H

#include "bRenderer.h"
#include "Entity.h"
#include "ProceduralOBJLoader.h"

class Sun : public Entity
{

public:
    Sun();
	Sun(std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer & renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale);

	void setIntensity(float intensity);

	void increaseIntensity(float dI);
    
    void updateSunIntensityInShader(std::string shaderName, float intensity);
    
    float getSunIntensity();
    
    void increaseSunSize(float deltaScale);

    float getHealth();
    void setHealth(float health);
    void increaseHealth(float hx);

	void setPosition(vmml::Vector3f position);

	float getVertexPos();

	void render(std::string camera, vmml::Vector3f playerPos, vmml::Matrix4f viewMatrixHUD, const double &elapsedTime);

private:
	ProceduralOBJLoader _objLoader;
	ModelData::GroupMap _data;
	ModelPtr _sunModel;
	Renderer _renderer;
	MaterialPtr _sunMaterial;
	PropertiesPtr _sunProperties;
	ModelPtr _sunFragments;
	ShaderPtr _shader;
	vmml::Vector3f _lightPosition;

	ModelData::GroupMap createFragments();
    
    float _health = 0.0;
    float _sunIntensity = 0.25;

	void renderFragments(std::string camera, vmml::Vector3f pos);

	void renderFragments(std::string camera);
    
    double _shaderOffset = 0.0;
    float _sunSize = 1.0;
    float _pulsateMin = 0.9;
    float _pulsateMax = 1.0;

};
typedef std::shared_ptr< Sun >  SunPtr;
#endif /* defined(B_SUN_H) */
