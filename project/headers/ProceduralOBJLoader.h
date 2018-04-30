#ifndef B_PROCEDURAL_OBJ_LOADER_H
#define B_PROCEDURAL_OBJ_LOADER_H

#include "bRenderer.h"
class ProceduralOBJLoader : public OBJLoader
{
public:
	ProceduralOBJLoader();

	void addVertex(float x, float y, float z);

	void addTexCoords(float u, float v);

	void addColor(float r, float g, float b, float a);

	void addFace(IndexData d1, IndexData d2, IndexData d3);

	void addFaceNoTex(IndexData d1, IndexData d2, IndexData d3);

	void printVertices();

	void printFaces();

	void printNormals();

	bool load();

	bool flatNormals();

	ModelData::GroupMap getData() const;

private:
	int _numFaces;

};
typedef std::shared_ptr< ProceduralOBJLoader >  ProceduralOBJLoaderPtr;
#endif /* defined(B_PROCEDURAL_OBJ_LOADER_H) */