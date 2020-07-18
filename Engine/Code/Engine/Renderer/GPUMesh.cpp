#include "GPUMesh.hpp"

GPUMesh::GPUMesh( RenderContext* ctx )
    : m_Owner( ctx )
{
    m_VertexBuffer = new VertexBuffer<Vertex_PCUTBN>( ctx, RenderMemoryHint::MEMORY_HINT_DYNAMIC);
    m_IndexBuffer = new IndexBuffer( ctx, RenderMemoryHint::MEMORY_HINT_DYNAMIC );
}

GPUMesh::~GPUMesh()
{
    if( m_VertexBuffer != nullptr  )
    {
        delete m_VertexBuffer;
        m_VertexBuffer = nullptr;
    }

    if( m_IndexBuffer != nullptr )
    {
        delete m_IndexBuffer;
        m_IndexBuffer = nullptr;
    }
}

size_t GPUMesh::GetVertexCount() const
{
    if( m_VertexBuffer == nullptr )
    {
        return 0;
    }
    return m_VertexBuffer->GetLocalSize();
}

size_t GPUMesh::GetIndexCount() const
{
    if( m_IndexBuffer == nullptr )
    {
        return 0;
    }
    return m_IndexBuffer->GetLocalSize();
}

void GPUMesh::UpdateIndexes( const std::vector<unsigned int>& indexes )
{
    m_IndexBuffer->AppendLocalBuffer( indexes );
}

void GPUMesh::UpdateIndexes( const unsigned int* indexes, const unsigned int count )
{
    m_IndexBuffer->AppendLocalBuffer( indexes, count );
}

void GPUMesh::UpdateVertexes( const std::vector<Vertex_PCUTBN>& vertexes )
{
    VertexBuffer<Vertex_PCUTBN>* test = VertexBuffer<Vertex_PCUTBN>::FromRenderBuffer( m_VertexBuffer );
    test->AppendLocalBuffer( vertexes );
}

void GPUMesh::UpdateVertexes( const Vertex_PCUTBN* vertexes, const unsigned int count )
{
    VertexBuffer<Vertex_PCUTBN>* test = VertexBuffer<Vertex_PCUTBN>::FromRenderBuffer( m_VertexBuffer );
    test->AppendLocalBuffer( vertexes, count );
}
