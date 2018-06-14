#include "Tree.h"
#include "iostream"

void Tree::draw(std::string camera)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), "camera", getModelMatrix(), std::vector<std::string>({"sun", "torch"}), true, true);
}

void Tree::render(std::string camera)
{
	// set ambient color
	renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));

	// draw model instance
	renderer().getModelRenderer()->queueModelInstance(getModelName(), _treeName, camera, getModelMatrix(), std::vector<std::string>({ "sun", "torch" }), true, true);
}

void Tree::customRender(std::string camera, vmml::Matrix4f view ,vmml::Matrix4f proj)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    
    // draw model
    renderer().getModelRenderer()->drawModel(renderer().getObjects()->getModel(getModelName()), computeTransformationMatrix(), view, proj, std::vector<std::string>({}), false);
}

void Tree::drawNormalsOnly(std::string camera, const double &deltaTime, std::string entityName)
{
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), camera, getModelMatrix(), std::vector<std::string>({ "sun" }), true, true);
}
