#include "Tree.h"
#include "iostream"

void Tree::add()
{
	getModel()->addInstance(_treeName);
}

void Tree::render(std::string camera)
{
	// set ambient color
	renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
	//renderer().getModelRenderer()->drawModelInstanced(getModelName(), camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);

	// draw model	
	//getModel()->addInstance(_treeName);
	//glDrawElementsInstancedEXT(mode, _nIndices, GL_UNSIGNED_SHORT, _indexData.get(), );
	renderer().getModelRenderer()->queueModelInstance(getModelName(), _treeName, camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
}
