#ifndef B_PROCEDURAL_OBJ_LOADER_H
#define B_PROCEDURAL_OBJ_LOADER_H

#include "bRenderer.h"
class ProceduralOBJLoader : public OBJLoader
{
public:
	ProceduralOBJLoader();

	void addVertex(float x, float y, float z);

	void addFace(int v1, int v2, int v3);

	void printVertices();

	void printFaces();

private:
	std::vector<FaceData>       _faces;
	std::vector<VertexData>     _vertices;

};
typedef std::shared_ptr< ProceduralOBJLoader >  ProceduralOBJLoaderPtr;
#endif /* defined(B_PROCEDURAL_OBJ_LOADER_H) */