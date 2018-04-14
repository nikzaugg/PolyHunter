#include "ProceduralOBJLoader.h"

ProceduralOBJLoader::ProceduralOBJLoader() 
	: OBJLoader(obj::obj_parser::parse_blank_lines_as_comment |
				obj::obj_parser::triangulate_faces |
				obj::obj_parser::translate_negative_indices)
{
	std::cout << "PROCEDURAL OBJ LOADER WORKS!!!" << std::endl;
}

void ProceduralOBJLoader::addVertex(float x, float y, float z)
{
	//Vertex v;

	//_group->vboVertices.push_back(v);
	//_group->vboIndices.push_back(_group->vboIndices.size());
	this->_vertices.push_back(vmml::Vector3f(x, y, z));
}

void ProceduralOBJLoader::addFace(int v1, int v2, int v3)
{
	FaceData f;
	f.v1 = v1;
	f.v2 = v2;
	f.v3 = v3;

	this->_faces.push_back(f);
}

void ProceduralOBJLoader::printVertices()
{
	for (int i = 0; i < this->_vertices.size(); i++)
	{
		std::cout << this->_vertices[i].position << std::endl;
	}
}

void ProceduralOBJLoader::printFaces()
{
	for (int i = 0; i < this->_faces.size(); i++)
	{
		std::cout << vmml::Vector3i(this->_faces[i].v1, this->_faces[i].v2, this->_faces[i].v3) << std::endl;
	}
}