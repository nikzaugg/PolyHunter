#ifndef B_TREE_H
#define B_TREE_H

#include "bRenderer.h"
#include "Entity.h"

class Tree : public Entity
{

public:
	Tree() : Entity() {};
    Tree(std::string treeName, std::string objName, std::string modelName, std::string propName, ShaderPtr shader, Renderer renderer, vmml::Vector3f pos, float rotX, float rotY, float rotZ, float scale) : Entity(objName, modelName, propName, shader, renderer, pos, rotX, rotY, rotZ, scale)
    {
        _treeName = treeName;
    };

    void draw(std::string camera);
	void render(std::string camera);
    void customRender(std::string camera, vmml::Matrix4f view ,vmml::Matrix4f proj);

private:
	std::string _treeName;

};
typedef std::shared_ptr< Tree >  TreePtr;
#endif /* defined(B_TREE_H) */
