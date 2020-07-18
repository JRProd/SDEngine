#pragma once
#include "Engine/Core/VertexTypes/Vertex_PCUTBN.hpp"
#include "Engine/IO/ObjFileUtils.hpp"


#include "Buffers/IndexBuffer.hpp"
#include "Buffers/VertexBuffer.hpp"

struct MeshLoadOptions;
class RenderContext;
class RenderBuffer;

struct LineSeg3D;
typedef float(SurfaceFunction)( const Vec2& uv );

struct BufferAttribute;

//TODO: Template to support vertex_pcu and vertex_pcutbn
class GPUMesh
{
public:
    GPUMesh( RenderContext* ctx );
    ~GPUMesh();

    static GPUMesh* CreateFromFile( RenderContext* ctx, const std::string& fileName, const MeshLoadOptions& meshLoadOptions = MeshLoadOptions() );

    static GPUMesh* CreateCube( RenderContext* ctx, const Vec3& halfSize,
                                const Rgba8& tint = Rgba8::WHITE );
    static GPUMesh* CreateLine( RenderContext* ctx, const LineSeg3D& lineSeg, float thickness, 
                                const Rgba8& tint = Rgba8::WHITE );
    static GPUMesh* CreatePlane( RenderContext* ctx, const Vec2& halfSize,
                                 const Vec2& splits = Vec2( 3.f, 3.f ),
                                 const Rgba8& tint = Rgba8::WHITE );
    static GPUMesh* CreatePlane( RenderContext* ctx, const Vec2& halfSize,
                                 SurfaceFunction* heightFunction,
                                 const Vec2& splits = Vec2( 3.f, 3.f ),
                                 const Rgba8& tint = Rgba8::WHITE );
    static GPUMesh* CreateSphereUv( RenderContext* ctx, const float radius, const Vec2& splits,
                                    const Rgba8& tint = Rgba8::WHITE);

    size_t GetVertexCount() const;
    size_t GetIndexCount() const;

    VertexBuffer<Vertex_PCUTBN>* GetVertexBuffer() const { return m_VertexBuffer; }
    IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; }

    void UpdateVertexes( const std::vector<Vertex_PCUTBN>& vertexes );
    void UpdateVertexes( const Vertex_PCUTBN* vertexes, unsigned int count );

    void UpdateIndexes( const std::vector<unsigned int>& indexes );
    void UpdateIndexes( const unsigned int* indexes, unsigned int count );

private:
    RenderContext* m_Owner = nullptr;
    VertexBuffer<Vertex_PCUTBN>* m_VertexBuffer = nullptr;
    IndexBuffer* m_IndexBuffer = nullptr;

};