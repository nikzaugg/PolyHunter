#ifndef B_GEOMETRY_DATA_H
#define B_GEOMETRY_DATA_H

#include <vector>
#include "Renderer_GL.h"
#include "vmmlib/vector.hpp"
#include "MaterialData.h"
#include "Texture.h"

struct IndexData
{
    GLushort vertexIndex;
    GLushort texCoordsIndex;
    GLushort normalIndex;
	GLushort colorIndex;
    
    IndexData()
    : vertexIndex(0)
    , texCoordsIndex(0)
    , normalIndex(0)
	, colorIndex(0)
    {}

	IndexData(GLushort vIndex, GLushort tIndex, GLushort nIndex)
		: vertexIndex(vIndex)
		, texCoordsIndex(tIndex)
		, normalIndex(nIndex)
		, colorIndex(0)
	{}

	IndexData(GLushort vIndex, GLushort tIndex, GLushort nIndex, GLushort cIndex)
		: vertexIndex(vIndex)
		, texCoordsIndex(tIndex)
		, normalIndex(nIndex)
		, colorIndex(cIndex)
	{}
};

struct TexCoord
{
    GLfloat s;
    GLfloat t;
};

struct Color
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
};

struct Point3
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct Vector3
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct Vertex
{
	Vertex()
	{
		position.x = 0.0f;
		position.y = 0.0f;
		position.z = 0.0f;
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;
		tangent.x = 0.0f;
		tangent.y = 0.0f;
		tangent.z = 0.0f;
		bitangent.x = 0.0f;
		bitangent.y = 0.0f;
		bitangent.z = 0.0f;
		texCoord.s = 0.0f;
		texCoord.t = 0.0f;
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
		color.a = 0.0f;
	}

	Vertex(GLfloat pX, GLfloat pY, GLfloat pZ, GLfloat tS, GLfloat tT)
	{
		position.x = pX;
		position.y = pY;
		position.z = pZ;
		normal.x = 0.0f;
		normal.y = 0.0f;
		normal.z = 0.0f;
		tangent.x = 0.0f;
		tangent.y = 0.0f;
		tangent.z = 0.0f;
		bitangent.x = 0.0f;
		bitangent.y = 0.0f;
		bitangent.z = 0.0f;
		texCoord.s = tS;
		texCoord.t = tT;
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
		color.a = 0.0f;
	}

	Vertex(GLfloat pX, GLfloat pY, GLfloat pZ, GLfloat nX, GLfloat nY, GLfloat nZ, GLfloat tX, GLfloat tY, GLfloat tZ, GLfloat bX, GLfloat bY, GLfloat bZ, GLfloat tS, GLfloat tT)
	{
		position.x = pX;
		position.y = pY;
		position.z = pZ;
		normal.x = nX;
		normal.y = nY;
		normal.z = nZ;
		tangent.x = tX;
		tangent.y = tY;
		tangent.z = tZ;
		bitangent.x = bX;
		bitangent.y = bY;
		bitangent.z = bZ;
		texCoord.s = tS;
		texCoord.t = tT;
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
		color.a = 0.0f;
	}

	Vertex(GLfloat pX, GLfloat pY, GLfloat pZ, GLfloat nX, GLfloat nY, GLfloat nZ, GLfloat tX, GLfloat tY, GLfloat tZ, GLfloat bX, GLfloat bY, GLfloat bZ, GLfloat tS, GLfloat tT, GLubyte r, GLubyte g, GLubyte b, GLubyte a)
	{
		position.x = pX;
		position.y = pY;
		position.z = pZ;
		normal.x = nX;
		normal.y = nY;
		normal.z = nZ;
		tangent.x = tX;
		tangent.y = tY;
		tangent.z = tZ;
		bitangent.x = bX;
		bitangent.y = bY;
		bitangent.z = bZ;
		texCoord.s = tS;
		texCoord.t = tT;
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	Point3      position;
	Vector3     normal;
	Vector3     tangent;
	Vector3     bitangent;
	TexCoord    texCoord;
	Color		color;
};

typedef GLushort Index;

/** @brief The underlying data of a geometry object.
*	@author David Steiner
*/
struct GeometryData
{
    typedef std::vector< Vertex >   VboVertices;
    typedef std::vector< GLushort > VboIndices;
    
    std::vector< IndexData > indices;
    
    VboVertices vboVertices;
    VboIndices  vboIndices;
    
    MaterialData materialData;
};

typedef std::shared_ptr< GeometryData > GeometryDataPtr;

#endif /* defined(B_GEOMETRY_DATA_H) */
