#include "Tree.h"
#include "iostream"

void Tree::draw(std::string camera)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), "camera", getModelMatrix(), std::vector<std::string>({"sun"}), true, true);
}

void Tree::render(std::string camera)
{
	// set ambient color
	renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    
//    MaterialPtr ssaoMat = renderer().getObjects()->loadObjMaterial("tree.mtl", "ssao_treeMaterial", renderer().getObjects()->getShader("swapShader"));
//    renderer().getObjects()->getModel(getModelName())->setMaterial(ssaoMat);
    
	// draw model instance
	renderer().getModelRenderer()->queueModelInstance(getModelName(), _treeName, camera, getModelMatrix(), std::vector<std::string>({ "sun" }), true, true);
}

void Tree::customRender(std::string camera, vmml::Matrix4f view ,vmml::Matrix4f proj)
{
    // set ambient color
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.5f));
    
    // draw model
    renderer().getModelRenderer()->drawModel(renderer().getObjects()->getModel(getModelName()), computeTransformationMatrix(), view, proj, std::vector<std::string>({}), false);
}

void Tree::drawPositionsOnly(std::string camera, const double &deltaTime, std::string entityName)
{
    renderer().getObjects()->getModel(getModelName())->setMaterial(renderer().getObjects()->getMaterial("tree_ssao_pos_depthMaterial"));
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    renderer().getObjects()->getShader("ssao_pos_depthShader")->setUniform("ModelMatrix", computeTransformationMatrix());
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
    // renderer().getObjects()->getModel(getModelName())->setMaterial(renderer().getObjects()->getMaterial("tree"));
}

void Tree::drawNormalsOnly(std::string camera, const double &deltaTime, std::string entityName)
{
    renderer().getObjects()->getModel(getModelName())->setMaterial(renderer().getObjects()->getMaterial("tree_ssao_normalMaterial"));
    renderer().getObjects()->setAmbientColor(vmml::Vector3f(0.3f));
    renderer().getObjects()->getShader("ssao_normalShader")->setUniform("ModelMatrix", computeTransformationMatrix());
    renderer().getObjects()->getShader("ssao_normalShader")->setUniform("flipNormal", 0.0);
    // draw model
    renderer().getModelRenderer()->drawModel(getModelName(), camera, computeTransformationMatrix(), std::vector<std::string>({ "sun" }), true, true);
    // renderer().getObjects()->getModel(getModelName())->setMaterial(renderer().getObjects()->getMaterial("tree_ssao_pos_depthMaterial"));
}



