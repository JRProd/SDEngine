#include "Vertex_PCU.hpp"

#include "Engine/Core/EngineCommon.hpp"

//-------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU()
    : VertexMaster()
{
}

Vertex_PCU::Vertex_PCU( const VertexMaster& copyFromMaster )
    : VertexMaster( copyFromMaster )
{
}

STATIC void Vertex_PCU::ConvertFromMaster( std::vector<Vertex_PCU>& output, const std::vector<VertexMaster>& input )
{
    for( const VertexMaster& master : input )
    {
        output.emplace_back( master );
    }
}

STATIC BufferAttribute Vertex_PCU::LAYOUT[] = {
    BufferAttribute( "POSITION", 0, BufferFormatType::VEC3, offsetof( Vertex_PCU, position ) ),
    BufferAttribute( "COLOR", 1, BufferFormatType::R8G8B8A8_UNORM, offsetof( Vertex_PCU, color ) ),
    BufferAttribute( "TEXCOORD", 2, BufferFormatType::VEC2, offsetof( Vertex_PCU, uv ) ),
    BufferAttribute()
};
