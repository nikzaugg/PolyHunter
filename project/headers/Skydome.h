#ifndef B_SKYDOME_H
#define B_SKYDOME_H

#include "bRenderer.h"

class Skydome
{
public:
	typedef std::unordered_map< std::string, GeometryPtr > GroupMap;
	
    Skydome(std::string modelName, Renderer & renderer);

	void setSkyColor(vmml::Vector3f color);
    void setSkydomeGradient(float gradient);
    void setSkydomeDensity(float density);
    void setSkydomeCenter(vmml::Vector3f center);
    void render(std::string camera, vmml::Vector3f position);

	vmml::Vector3f getSkyColor();

private:
	Renderer _renderer;
    ModelPtr _model;
    std::string _modelName;

	int _numFaces;

    vmml::Vector3f _skyColor;
    vmml::Vector3f _center;
	int _SIZE = 600;
    float _scale = 60.0;
    float _offsetBottom = 150.0;
    float _skydomeGradient = 0.04;
    float _skydomeDensity = 0.09;
};
typedef std::shared_ptr< Skydome >  SkydomePtr;
#endif /* defined(B_SKYDOME_H) */
