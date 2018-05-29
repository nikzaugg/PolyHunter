#include "Crystal.h"
#include "iostream"

void Crystal::draw(std::string camera)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), "camera", getModelMatrix(), std::vector<std::string>({"sun"}), true, true);
}

void Crystal::render(std::string camera)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    
    // draw model instance
    renderer().getModelRenderer()->queueModelInstance(getModelName(), _crystalName, camera, getModelMatrix(), std::vector<std::string>({ "sun" }), true, true);
}

void Crystal::customRender(std::string camera, vmml::Matrix4f view ,vmml::Matrix4f proj)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    
    // draw model
    renderer().getModelRenderer()->drawModel(renderer().getObjects()->getModel(getModelName()), computeTransformationMatrix(), view, proj, std::vector<std::string>({}), false);
}

