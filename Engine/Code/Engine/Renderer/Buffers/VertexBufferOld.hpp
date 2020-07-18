// #pragma once
//
// #include <vector>
//
// #include "Engine/Renderer/Buffers/RenderBuffer.hpp"
// #include "Engine/Renderer/RenderContext.hpp"
//
// struct BufferAttribute;
//
// template<class VertexType>
// class VertexBuffer: public RenderBuffer
// {
//     friend class RenderContext;
//
// public:
//     VertexBuffer( RenderContext* owner, RenderMemoryHint memHint);
//     virtual ~VertexBuffer() = default;
//
//     static VertexBuffer* FromRenderBuffer( RenderBuffer* renderBuffer );
//
//     const BufferAttribute* GetBufferAttributes() const { return VertexType::LAYOUT; }
//
//     void AppendLocalBuffer( const VertexType* vertexes, unsigned int size );
//     void AppendLocalBuffer( const std::vector<VertexType>& vertexes );
//
// private:
//     std::vector<VertexMaster> m_LocalBuffer;
//
//     void UpdateAndBind( unsigned int slot, unsigned int numBuffers = 1, unsigned int offset = 0 ) override;
// };
//
// template<class VertexType>
// VertexBuffer<VertexType>::VertexBuffer( RenderContext* owner, RenderMemoryHint memHint )
//     : RenderBuffer( owner, VERTEX_BUFFER_BIT, memHint )
// {
// }
//
// template<class VertexType>
// inline VertexBuffer<VertexType>* VertexBuffer<VertexType>::FromRenderBuffer( RenderBuffer* renderBuffer )
// {
//     VertexBuffer<VertexType>* vertexBuffer = dynamic_cast<VertexBuffer<VertexType>*>(renderBuffer);
//     GUARANTEE_OR_DIE( vertexBuffer != nullptr, "VertexBuffer::FromRenderBuffer - The buffer is not a vertex buffer" );
//
//     return vertexBuffer;
// }
//
// template<class VertexType>
// void VertexBuffer<VertexType>::AppendLocalBuffer( const VertexType* vertexes, unsigned int size )
// {
//     m_LocalBuffer.reserve( m_LocalBuffer.size() + size );
//
//     for ( size_t index = 0; index < size; ++index )
//     {
//         m_LocalBuffer.push_back( vertexes[ index ] );
//     }
// }
//
// template<class VertexType>
// void VertexBuffer<VertexType>::AppendLocalBuffer( const std::vector<VertexType>& vertexes )
// {
//     m_LocalBuffer.insert( m_LocalBuffer.end(), vertexes.cbegin(), vertexes.cend() );
// }
//
// template<class VertexType>
// void VertexBuffer<VertexType>::UpdateAndBind( unsigned int slot, unsigned int numBuffers, unsigned int offset )
// {
//     UNUSED( slot );
//     UNUSED( numBuffers );
//     UNUSED( offset );
//
//
//     // Update
//     const unsigned int elementSize = sizeof( VertexType );
//     const unsigned int bufferSize = static_cast<unsigned int>( m_LocalBuffer.size() * elementSize );
//     const bool success = Update( m_LocalBuffer.data(), bufferSize, elementSize );
//
//     if( !success )
//     {
//         // m_Owner->m_Context->IASetVertexBuffers( slot, numBuffers, &m_Handle, &elementSize, &offset);
//     }
//
//     m_IsDirty = false;
//     m_LocalBuffer.clear();
// }
