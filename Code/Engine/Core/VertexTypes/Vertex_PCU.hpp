#pragma once

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Renderer/Buffers/BufferAttribute.hpp"

#include <vector>

struct Vertex_PCU: public VertexMaster
{

public:
    Vertex_PCU();
    Vertex_PCU( const VertexMaster& copyFromMaster );

    static void ConvertFromMaster( std::vector<Vertex_PCU>& output, const std::vector<VertexMaster>& input );
    static BufferAttribute LAYOUT[];
};