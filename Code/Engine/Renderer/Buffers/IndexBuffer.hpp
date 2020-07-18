#pragma once

#include "RenderBuffer.hpp"

class IndexBuffer: public RenderBuffer
{
    friend class RenderContext;

public:
    IndexBuffer( RenderContext* owner, RenderMemoryHint memHint );
    virtual ~IndexBuffer() = default;

    void AppendLocalBuffer( const unsigned int* indexes, unsigned int size );
    void AppendLocalBuffer( const std::vector<unsigned int>& indexes );
    void AppendLocalBuffer( const std::vector<unsigned int>& indexes, size_t start );
    void AppendLocalBuffer( size_t amount, size_t start );

private:
    std::vector<unsigned int> m_LocalBuffer;

    void UpdateAndBind( unsigned int slot = 0, unsigned int numBuffers = 1, unsigned int offset = 0 ) override;
};