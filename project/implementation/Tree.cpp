#include "Tree.h"
#include "iostream"

void Tree::render(std::string camera)
{
	// set ambient color
	renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));

	// draw model
	renderer().getModelRenderer()->queueModelInstance(getModelName(), _treeName, camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}