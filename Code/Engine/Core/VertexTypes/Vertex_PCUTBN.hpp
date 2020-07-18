#pragma once

#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Renderer/Buffers/BufferAttribute.hpp"

#include <vector>

struct Vertex_PCUTBN: public VertexMaster
{

public:
    Vertex_PCUTBN();
    Vertex_PCUTBN( const VertexMaster& copyFromMaster );

    static void ConvertFromMaster( std::vector<Vertex_PCUTBN>& output, const std::vector<VertexMaster>& input );
    static BufferAttribute LAYOUT[];
};