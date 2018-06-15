#ifndef B_SKYDOME_H
#define B_SKYDOME_H

#include "bRenderer.h"
typedef struct {
	vmml::Vector3f position;
	float scale;
	int type;
} Cloud;

class Skydome
{
public:
	typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	typedef std::unordered_map< std::string, Cloud > CloudMap;
	
    Skydome(std::string modelName, Renderer & renderer);

	void setSkyColor(vmml::Vector3f color);
    void setSkydomeGradient(float gradient);
    void setSkydomeDensity(float density);
    void setSkydomeCenter(vmml::Vector3f center);
    void render(std::string camera, vmml::Vector3f position);
    void reset();

	vmml::Vector3f getSkyColor();

	void renderClouds(std::string camera, vmml::Vector3f position);

private:
	Renderer _renderer;
    ModelPtr _model;
    std::string _modelName;
	CloudMap _cloudMap;

	int _numFaces;

    vmml::Vector3f _skyColor;
    vmml::Vector3f _center;
	int _SIZE = 600;
    float _scale = 60.0;
    float _offsetBottom = 150.0;
    float _skydomeGradient = 0.04;
    float _skydomeDensity = 0.09;

    int getRandomIntInRange(int lower, int upper);
	void createClouds();
};
typedef std::shared_ptr< Skydome >  SkydomePtr;
#endif /* defined(B_SKYDOME_H) */
