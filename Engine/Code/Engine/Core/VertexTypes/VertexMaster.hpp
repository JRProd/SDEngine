#pragma once

#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"

#include <vector>

struct Mat44;

struct VertexMaster
{
    Vec3 position;
    Rgba8 color;
    Vec2 uv;

    Vec3 tangent;
    Vec3 bitangent;
    Vec3 normal;

public:
    VertexMaster() = default;
    explicit VertexMaster( const Vec2& pos, const Rgba8& col, const Vec2& uvs = Vec2::Vec2::ZERO );
    explicit VertexMaster( const Vec3& pos, const Rgba8& col, const Vec2& uvs = Vec2::Vec2::ZERO );
    explicit VertexMaster( const Vec3& pos, const Rgba8& col, const Vec2& uvs, const Vec3& tang,
                           const Vec3& bitang, const Vec3& norm );

    bool IsMostlyEqual( const VertexMaster& rhs, float epsilon = 1e-7f ) const;
};

void TransformVertexArray( std::vector<VertexMaster>& vertexes,
                           const Mat44& transform );

void TransformVertexArray( std::vector<VertexMaster>& vertexes,
                           const Vec3& transform,
                           const Vec3& rotation,
                           const Vec3& scale );

void TransformVertexArray( std::vector<VertexMaster>& vertexes,
                           const Vec2& transform,
                           float rotation,
                           const Vec2& scale );

void TransformVertexArray( std::vector<VertexMaster>& vertexes,
                           const Vec3& transform,
                           const Vec3& rotation,
                           float scale );

void TransformVertexArray( std::vector<VertexMaster>& vertexes,
                           const Vec2& transform,
                           float rotation,
                           float scale );
