#pragma once

#include <vector>

#include "Engine/Renderer/Buffers/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

struct BufferAttribute;

template<class VertexType>
class VertexBuffer: public RenderBuffer
{
    friend class RenderContext;

public:
    VertexBuffer( RenderContext* owner, RenderMemoryHint memHint );
     virtual ~VertexBuffer() = default;

     static VertexBuffer<VertexType>* FromRenderBuffer( RenderBuffer* renderBuffer );

     size_t AppendLocalBuffer( const VertexType* vertexes, unsigned int size );
     size_t AppendLocalBuffer( const std::vector<VertexType>& vertexes );

private:
     std::vector<VertexType> m_LocalBuffer;

     void UpdateAndBind( unsigned int slot, unsigned int numBuffers = 1, unsigned int offset = 0 ) override;
};
template<class VertexType>
inline VertexBuffer<VertexType>::VertexBuffer( RenderContext* owner, RenderMemoryHint memHint )
    : RenderBuffer( owner, VERTEX_BUFFER_BIT, memHint )
{
    m_ElementByteSize = sizeof( VertexType );
    m_BufferAttribute = VertexType::LAYOUT;
}
template<class VertexType>
VertexBuffer<VertexType>* VertexBuffer<VertexType>::FromRenderBuffer( RenderBuffer* renderBuffer )
{
    VertexBuffer* vertexBuffer = dynamic_cast<VertexBuffer<VertexType>*>(renderBuffer);
    GUARANTEE_OR_DIE( vertexBuffer != nullptr, "VertexBuffer::FromRenderBuffer - The buffer is not a vertex buffer" );

    return vertexBuffer;
}

template<class VertexType>
size_t VertexBuffer<VertexType>::AppendLocalBuffer( const VertexType* vertexes, unsigned int size )
{
    const size_t bufferStart = m_LocalBuffer.size();

    m_LocalBuffer.reserve( m_LocalBuffer.size() + size );

    for ( size_t index = 0; index < size; ++index )
    {
        m_LocalBuffer.push_back( vertexes[ index ] );
    }

    m_IsDirty = true;
    m_LocalSize = static_cast<unsigned int>(m_LocalBuffer.size());
    return bufferStart;
}


template<class VertexType>
size_t VertexBuffer<VertexType>::AppendLocalBuffer( const std::vector<VertexType>& vertexes )
{
    const size_t bufferStart = m_LocalBuffer.size();

    m_LocalBuffer.insert( m_LocalBuffer.end(), vertexes.cbegin(), vertexes.cend() );

    m_IsDirty = true;
    m_LocalSize = static_cast<unsigned int>(m_LocalBuffer.size());
    return bufferStart;
}

template<class VertexType>
void VertexBuffer<VertexType>::UpdateAndBind( unsigned int slot, unsigned int numBuffers, unsigned int offset )
{

    // Update
    const unsigned int elementSize = sizeof( VertexType );
    if( m_IsDirty )
    {
        const unsigned int bufferSize = static_cast<unsigned int>(GetLocalSize() * elementSize);
        Update( m_LocalBuffer.data(), bufferSize, elementSize );
    }

    m_Owner->m_Context->IASetVertexBuffers( slot, numBuffers, &m_Handle, &elementSize, &offset );

    m_LocalBuffer.clear();
    m_IsDirty = false;
}
