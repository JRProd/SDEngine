#include "Vertex_PCUTBN.hpp"

#include "Engine/Core/EngineCommon.hpp"

Vertex_PCUTBN::Vertex_PCUTBN()
    : VertexMaster( )
{
}

Vertex_PCUTBN::Vertex_PCUTBN( const VertexMaster& copyFromMaster )
    : VertexMaster( copyFromMaster )
{
}

STATIC void Vertex_PCUTBN::ConvertFromMaster( std::vector<Vertex_PCUTBN>& output, const std::vector<VertexMaster>& input )
{
    for( const VertexMaster& copy : input )
    {
        output.emplace_back( copy );
    }
}

STATIC BufferAttribute Vertex_PCUTBN::LAYOUT[] = {
    BufferAttribute( "POSITION", 0, BufferFormatType::VEC3, offsetof( Vertex_PCUTBN, position ) ),
    BufferAttribute( "COLOR", 1, BufferFormatType::R8G8B8A8_UNORM, offsetof( Vertex_PCUTBN, color ) ),
    BufferAttribute( "TEXCOORD", 2, BufferFormatType::VEC2, offsetof( Vertex_PCUTBN, uv ) ),
    BufferAttribute( "TANGENT", 3, BufferFormatType::VEC3, offsetof( Vertex_PCUTBN, tangent ) ),
    BufferAttribute( "BITANGENT", 4, BufferFormatType::VEC3, offsetof( Vertex_PCUTBN, bitangent ) ),
    BufferAttribute( "NORMAL", 5, BufferFormatType::VEC3, offsetof( Vertex_PCUTBN, normal ) ),
    BufferAttribute()
};
