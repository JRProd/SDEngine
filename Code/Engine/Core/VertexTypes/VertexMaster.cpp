#include "VertexMaster.hpp"


#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Transform.hpp"

VertexMaster::VertexMaster( const Vec2& pos, const Rgba8& col, const Vec2& uvs )
    : VertexMaster( Vec3(pos), col, uvs )
{
}

VertexMaster::VertexMaster( const Vec3& pos, const Rgba8& col, const Vec2& uvs )
    : VertexMaster( pos, col, uvs, Vec3(), Vec3(), Vec3() )
{
}

VertexMaster::VertexMaster( const Vec3& pos, const Rgba8& col, const Vec2& uvs, const Vec3& tang,
                            const Vec3& bitang, const Vec3& norm )
    : position( pos )
      , color( col )
      , uv( uvs )
      , tangent( tang )
      , bitangent( bitang )
      , normal( norm )
{
}

bool VertexMaster::IsMostlyEqual( const VertexMaster& rhs, const float epsilon ) const
{
    return this->position.IsMostlyEqual( rhs.position, epsilon) && this->uv.IsMostlyEqual( rhs.uv, epsilon) && 
        (this->color == rhs.color) && this->tangent.IsMostlyEqual( rhs.tangent, epsilon ) &&
        this->bitangent.IsMostlyEqual( rhs.bitangent, epsilon ) && this->normal.IsMostlyEqual( rhs.normal, epsilon );
}

void TransformVertexArray( std::vector<VertexMaster>& vertexes, const Mat44& transform )
{
    for( VertexMaster& vertex : vertexes )
    {
        vertex.position = transform.TransformPosition(vertex.position);
    }
}

void TransformVertexArray( std::vector<VertexMaster>& vertexes, const Vec3& transform, const Vec3& rotation, const Vec3& scale )
{
    Mat44 matrixTransform = Transform( transform, scale, rotation ).GetAsMatrixWithCanonicalTransform();
    matrixTransform.PushMatrix( Engine::GetCanonicalTransformInverse() );
    TransformVertexArray( vertexes, matrixTransform );
}

void TransformVertexArray( std::vector<VertexMaster>& vertexes, const Vec2& transform, const float rotation, const Vec2& scale )
{
    TransformVertexArray( vertexes, Vec3( transform ), Vec3( 0.f, 0.f, rotation ), Vec3( scale, 1.f ) );
}

void TransformVertexArray( std::vector<VertexMaster>& vertexes, const Vec3& transform, const Vec3& rotation, float scale )
{
    TransformVertexArray( vertexes, transform, rotation, Vec3( scale ) );
}

void TransformVertexArray( std::vector<VertexMaster>& vertexes, const Vec2& transform, const float rotation, float scale )
{
    TransformVertexArray( vertexes, transform, rotation, Vec2( scale ) );
}
