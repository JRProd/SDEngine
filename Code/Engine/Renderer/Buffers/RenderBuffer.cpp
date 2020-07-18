#include "RenderBuffer.hpp"

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

D3D11_USAGE ToDXMemoryUsage( RenderMemoryHint hint )
{
    switch ( hint )
    {
    case RenderMemoryHint::MEMORY_HINT_GPU: return D3D11_USAGE_DEFAULT;
    case RenderMemoryHint::MEMORY_HINT_DYNAMIC: return D3D11_USAGE_DYNAMIC;
    case RenderMemoryHint::MEMORY_HINT_STATGING: return D3D11_USAGE_STAGING;
    default: ERROR_AND_DIE( "RenderMemoryHint: Hint not supported" );
    }
}

unsigned int ToDXUsage( RenderBufferUsage usage )
{
    unsigned int ret = 0;

    if ( usage & VERTEX_BUFFER_BIT )
    {
        ret |= D3D11_BIND_VERTEX_BUFFER;
    }

    if ( usage & INDEX_BUFFER_BIT )
    {
        ret |= D3D11_BIND_INDEX_BUFFER;
    }

    if ( usage & UNIFORM_BUFFER_BIT )
    {
        ret |= D3D11_BIND_CONSTANT_BUFFER;
    }

    return ret;
}

RenderBuffer::RenderBuffer( RenderContext* owner, RenderBufferUsage usage,
                            RenderMemoryHint memHint )
    : m_Owner( owner )
      , m_Usage( usage )
      , m_MemoryHint( memHint )
{
}

RenderBuffer::~RenderBuffer()
{
    m_Owner = nullptr;
    DX_SAFE_RELEASE( m_Handle );
}

void RenderBuffer::Update( const void* data, unsigned int dataByteSize, unsigned int elementByteSize )
{
    if ( !IsCompatible( dataByteSize, elementByteSize ) )
    {
        Cleanup();
        Create( dataByteSize, elementByteSize );
    }

    // 3. Update 
    ID3D11DeviceContext* ctx = m_Owner->m_Context;
    if ( m_MemoryHint == RenderMemoryHint::MEMORY_HINT_DYNAMIC )
    {
        // Mapping
        // Only available to DYNAMIC buffer
        D3D11_MAPPED_SUBRESOURCE mapped;

        HRESULT result = ctx->Map( m_Handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
        if ( SUCCEEDED( result ) )
        {
            memcpy( mapped.pData, data, dataByteSize );
            ctx->Unmap( m_Handle, 0 );
        }
    }
    else
    {
        ctx->UpdateSubresource( m_Handle, 0, nullptr, data, 0, 0 );
    }
}

void RenderBuffer::MakeDirty()
{
    m_IsDirty = true;
}

void RenderBuffer::MakeClean()
{
    m_IsDirty = false;
}

unsigned int RenderBuffer::GetLocalSize() const
{
    return m_LocalSize;
}

bool RenderBuffer::Create( unsigned int dataByteSize, unsigned int elementByteSize )
{
    ID3D11Device* device = m_Owner->m_Device;
    D3D11_BUFFER_DESC desc;

    m_BufferByteSize = dataByteSize;
    m_ElementByteSize = elementByteSize;

    desc.ByteWidth = static_cast<unsigned int>(dataByteSize);
    desc.Usage = ToDXMemoryUsage( m_MemoryHint );
    desc.BindFlags = ToDXUsage( m_Usage );
    if ( m_MemoryHint == RenderMemoryHint::MEMORY_HINT_DYNAMIC )
    {
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else if ( m_MemoryHint == RenderMemoryHint::MEMORY_HINT_STATGING )
    {
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    }

    desc.MiscFlags = 0;
    desc.StructureByteStride = static_cast<unsigned int>(elementByteSize);

    device->CreateBuffer( &desc, nullptr, &m_Handle );

    return true;
}

bool RenderBuffer::IsCompatible( unsigned int dataByteSize, unsigned int elementByteSize ) const
{
    if ( m_Handle == nullptr )
    {
        return false;
    }

    if ( m_ElementByteSize != elementByteSize )
    {
        return false;
    }

    if ( m_MemoryHint == RenderMemoryHint::MEMORY_HINT_DYNAMIC )
    {
        return dataByteSize <= m_BufferByteSize;
    }

    return dataByteSize == m_BufferByteSize;
}

void RenderBuffer::Cleanup()
{
    DX_SAFE_RELEASE( m_Handle );

    m_BufferByteSize = 0;
    m_ElementByteSize = 0;

    m_IsDirty = true;
}

void RenderBuffer::Flush()
{
    m_LocalSize = 0;
}




