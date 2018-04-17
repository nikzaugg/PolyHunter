#include "ProceduralOBJLoader.h"
#include "OBJLoader.h"

ProceduralOBJLoader::ProceduralOBJLoader()
: OBJLoader(obj::obj_parser::parse_blank_lines_as_comment |
obj::obj_parser::triangulate_faces |
            obj::obj_parser::translate_negative_indices)
{
    std::cout << "PROCEDURAL OBJ LOADER WORKS!!!" << std::endl;
    ModelData dataPtr = ModelData(false, true);
    _data = &dataPtr;
    createGroup(bRenderer::DEFAULT_GROUP_NAME());
    //createGroup("terrain");
    _numFaces = 0;
}

void ProceduralOBJLoader::addVertex(float x, float y, float z)
{
    _vertices.push_back(vmml::Vector3f(x, y, z));
}

void ProceduralOBJLoader::addTexCoords(float u, float v)
{
    _texCoords.push_back(vmml::Vector2f(u, v));
}

void ProceduralOBJLoader::addFace(IndexData d1, IndexData d2, IndexData d3)
{
    _group->indices.push_back(d1);
    _group->indices.push_back(d2);
    _group->indices.push_back(d3);
    
    genVertex< true, true, false >(d1);
    genVertex< true, true, false >(d2);
    genVertex< true, true, false >(d3);
    
    genFace< false >(d1, d2, d3);
}

void ProceduralOBJLoader::printVertices()
{
    for (int i = 0; i < _vertices.size(); ++i)
    {
        std::cout << _vertices[i].position << std::endl;
    }
}

void ProceduralOBJLoader::printFaces()
{
    for (int i = 0; i < _faces.size(); ++i)
    {
        std::cout << vmml::Vector3i(_faces[i].v1, _faces[i].v2, _faces[i].v3) << std::endl;
    }
}

void ProceduralOBJLoader::printNormals()
{
    for (FaceData &face : _faces)
    {
        std::cout << face.normal << std::endl;
    }
}

bool ProceduralOBJLoader::load()
{
    
    // if there is not only vertex- but also face data, update normals accordingly
    if (_faces.size() > 0)
    {
        createFaceNormals();
        createVertexNormals();
        
        for (auto i = _groups.begin(); i != _groups.end(); ++i)
        {
            for (auto j = i->second->vboIndices.begin(); j != i->second->vboIndices.end(); ++j)
            {
                auto idx = i->second->indices[*j].vertexIndex;
                
                vmml::Vector3f cNormal = _vertices[idx].normal;
                Vector3 &normal = i->second->vboVertices[*j].normal;
                normal.x = cNormal.x();
                normal.y = cNormal.y();
                normal.z = cNormal.z();
                
                vmml::Vector3f cTangent = _vertices[idx].tangent;
                Vector3 &tangent = i->second->vboVertices[*j].tangent;
                tangent.x = cTangent.x();
                tangent.y = cTangent.y();
                tangent.z = cTangent.z();
                
                vmml::Vector3f cBitangent = _vertices[idx].bitangent;
                Vector3 &bitangent = i->second->vboVertices[*j].bitangent;
                bitangent.x = cBitangent.x();
                bitangent.y = cBitangent.y();
                bitangent.z = cBitangent.z();
            }
        }
    }
    
    // delete empty groups
    auto i = _groups.begin();
    while (i != _groups.end())
    {
        if (i->second->vboIndices.size() == 0 || i->second->vboVertices.size() == 0)
            _groups.erase(i++);
        else
            ++i;
    }
    
    return true;
}

ModelData::GroupMap ProceduralOBJLoader::getData() const
{
    return _groups;
}

