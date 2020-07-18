#pragma once

#include "Engine/Core/EngineCommon.hpp"

#include "BufferAttribute.hpp"

//-----------------------------------------------------------------------------
// D3D11 Predefines
struct ID3D11Buffer;
// Engine Predefines
class RenderContext;

// Defines what we are using the buffer for
typedef unsigned int RenderBufferUsage;

enum RenderBufferUsageBit: unsigned int
{
    VERTEX_BUFFER_BIT = BIT_FLAG<0>,      // A02 - (VBO)
    INDEX_BUFFER_BIT = BIT_FLAG<1>,       // A05 - (IBO)
    UNIFORM_BUFFER_BIT = BIT_FLAG<2>,     // A03 - (UBO)
};

// Defines how we will access it
enum class RenderMemoryHint: unsigned int
{
    MEMORY_HINT_GPU,        // Gpu can read/write, cpu can't touch it. If we change it, it changes rarely.
    MEMORY_HINT_DYNAMIC,    // Gpu memory (read/write), that changes OFTEN form the CPU - it allows us to 'Map' the memory.
    MEMORY_HINT_STATGING,   // Cpu read/write, can copy from cpu->gpu.
};

class RenderBuffer
{
    friend class RenderContext;

public:
    RenderContext* m_Owner = nullptr;
    ID3D11Buffer* m_Handle = nullptr;

    RenderBuffer( RenderContext* owner, RenderBufferUsage usage, RenderMemoryHint memHint );
    virtual ~RenderBuffer();

    bool IsDirty() const { return m_IsDirty; }
    void MakeDirty();
    void MakeClean();

    unsigned int GetLocalSize() const;
    unsigned int GetElementSize() const { return m_ElementByteSize; }

protected:
    unsigned int m_LocalSize = 0U;
    unsigned int m_ElementByteSize = 0U;

    bool m_IsDirty = true;
    BufferAttribute* m_BufferAttribute;

    void Update( const void* data, unsigned int dataByteSize, unsigned int elementByteSize );

private:
    RenderBufferUsage m_Usage;
    RenderMemoryHint m_MemoryHint;

    unsigned int m_BufferByteSize = 0U;

    virtual void UpdateAndBind( unsigned int slot, unsigned int numBuffers = 1, unsigned int offset = 0 ) = 0;

    bool Create( unsigned int dataByteSize, unsigned int elementByteSize );
    bool IsCompatible( unsigned int dataByteSize, unsigned int elementByteSize ) const;
    void Cleanup();

    void Flush();
};