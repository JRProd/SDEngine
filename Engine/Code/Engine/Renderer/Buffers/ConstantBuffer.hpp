#pragma once

#include "Engine/Renderer/Buffers/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

template< class BufferStruct>
class ConstantBuffer: public RenderBuffer
{
    friend class RenderContext;

public:
    ConstantBuffer( RenderContext* owner );

    static ConstantBuffer<BufferStruct>* FromRenderBuffer( RenderBuffer* renderBuffer );

    const BufferStruct& GetLocalBuffer() const;

    void UpdateLocalBuffer( const BufferStruct& updated );

    bool operator==( const BufferStruct& rhs ) const;

private:
    BufferStruct m_LocalBuffer;

    void UpdateAndBind( unsigned slot, unsigned numBuffers = 1, unsigned offset = 0 ) override;
};

template<class BufferStruct>
inline ConstantBuffer<BufferStruct>::ConstantBuffer( RenderContext* owner )
    : RenderBuffer( owner, UNIFORM_BUFFER_BIT, RenderMemoryHint::MEMORY_HINT_DYNAMIC )
{
    m_ElementByteSize = sizeof( BufferStruct );
}

template<class BufferStruct>
inline ConstantBuffer<BufferStruct>* ConstantBuffer<BufferStruct>::FromRenderBuffer( RenderBuffer* renderBuffer )
{

    ConstantBuffer<BufferStruct>* constantBuffer = dynamic_cast<ConstantBuffer<BufferStruct>*>( renderBuffer );
    GUARANTEE_OR_DIE( constantBuffer != nullptr, "ConstantBuffer::FromRenderBuffer - The buffer is not a constant buffer" );

    return constantBuffer;
}

template<typename BufferStruct>
const BufferStruct& ConstantBuffer<BufferStruct>::GetLocalBuffer() const
{
    return m_LocalBuffer;
}

template<typename BufferStruct>
inline void ConstantBuffer<BufferStruct>::UpdateLocalBuffer( const BufferStruct& updated )
{
    if ( m_LocalBuffer != updated )
    {
         m_IsDirty = true;
         m_LocalBuffer = updated;
    }
}

template<class BufferStruct>
inline bool ConstantBuffer<BufferStruct>::operator==( const BufferStruct& rhs ) const
{
    return m_LocalBuffer == rhs;
}

template<typename BufferStruct>
void ConstantBuffer<BufferStruct>::UpdateAndBind( unsigned slot, unsigned numBuffers, unsigned offset )
{
    UNUSED( offset );

    // Update
    Update( &m_LocalBuffer, sizeof( m_LocalBuffer ), sizeof( m_LocalBuffer ) );

    // Bind
    m_Owner->m_Context->VSSetConstantBuffers( slot, numBuffers, &m_Handle );
    m_Owner->m_Context->PSSetConstantBuffers( slot, numBuffers, &m_Handle );

    m_IsDirty = false;
}

