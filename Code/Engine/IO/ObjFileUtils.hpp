#pragma once
#include "Engine/Core/Math/Primatives/Mat44.hpp"
#include "Engine/Core/VertexTypes/VertexMaster.hpp"

struct MeshLoadOptions
{
    Mat44 transform = Mat44::IDENTITY;

    bool invertV = false;
    bool calculateNormals = true;
    bool calculateTangents = true;

    bool flipWindingOrder = false;
    bool clean = false;
};

void LoadFromObjFile( const std::string& fileName, std::vector<VertexMaster>& vertexes, 
                      std::vector<unsigned int>& indexes, 
                      const MeshLoadOptions& meshLoadOptions = MeshLoadOptions() );
