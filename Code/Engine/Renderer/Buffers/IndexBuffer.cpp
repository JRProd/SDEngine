#include "IndexBuffer.hpp"

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

IndexBuffer::IndexBuffer( RenderContext* owner, RenderMemoryHint memHint )
    : RenderBuffer( owner, INDEX_BUFFER_BIT, memHint )
{
    m_ElementByteSize = sizeof( unsigned int );
}

void IndexBuffer::AppendLocalBuffer( const unsigned int* indexes, unsigned int size )
{
    m_LocalBuffer.reserve( m_LocalBuffer.size() + size );

    for( size_t index = 0; index < size; ++index )
    {
        m_LocalBuffer.push_back( indexes[ index ] );
    }

    m_LocalSize = static_cast<unsigned int>(m_LocalBuffer.size());
    m_IsDirty = true;
}

void IndexBuffer::AppendLocalBuffer( const std::vector<unsigned int>& indexes )
{
    m_LocalBuffer.insert( m_LocalBuffer.end(), indexes.cbegin(), indexes.cend() );

    m_LocalSize = static_cast<unsigned int>(m_LocalBuffer.size());
    m_IsDirty = true;
}

void IndexBuffer::AppendLocalBuffer( const std::vector<unsigned int>& indexes, const size_t start )
{
    m_LocalBuffer.reserve( m_LocalBuffer.size() + indexes.size() );

    for( unsigned int index : indexes )
    {
        m_LocalBuffer.push_back( index + static_cast<unsigned int>(start) );
    }

    m_LocalSize = static_cast<unsigned int>(m_LocalBuffer.size());
    m_IsDirty = true;
}

void IndexBuffer::AppendLocalBuffer( const size_t amount, const size_t start )
{
    m_LocalBuffer.reserve( m_LocalBuffer.size() + amount );

    for ( size_t index = start; index < amount + start; ++index )
    {
        m_LocalBuffer.push_back( static_cast<unsigned int>(index) );
    }

    m_LocalSize = static_cast<unsigned int>(m_LocalBuffer.size());
    m_IsDirty = true;
}

void IndexBuffer::UpdateAndBind( unsigned int slot, unsigned int numBuffers, const unsigned int offset )
{
    UNUSED( slot );
    UNUSED( numBuffers );

    // Update
    if( m_IsDirty )
    {
        const unsigned int elementSize = sizeof( unsigned int );
        const unsigned int bufferSize = static_cast<unsigned int>(GetLocalSize() * elementSize);
        Update( m_LocalBuffer.data(), bufferSize, elementSize );
    }

    // Bind
    m_Owner->m_Context->IASetIndexBuffer( m_Handle, DXGI_FORMAT_R32_UINT, offset );

    m_LocalBuffer.clear();
    m_IsDirty = false;
}
