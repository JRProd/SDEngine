#pragma once

#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"

#include <vector>

struct LineSeg2D;
struct Mat44;
struct LineSeg3D;
struct AABB3;
struct AABB2;
struct Disc;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++                                          Import Steps                                   +++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct MikkTangentSupport
{
    std::vector<VertexMaster>* vertexes;
    std::vector<unsigned int>* indexes;

    bool useIndex = true;
};

void MikkTangentCalculation( std::vector<VertexMaster>& vertexes );
void MikkTangentCalculation( std::vector<VertexMaster>& vertexes,
                             std::vector<unsigned int>& indexes );

void CleanMesh( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes );

void IndexMesh( const std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes );

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++                                       Appending Functions                               +++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void AppendLine( std::vector<VertexMaster>& vertexes,
                 const LineSeg2D& lineSeg,
                 const Rgba8& startTint,
                 const Rgba8& endTint,
                 float thickness = 1.f );
void AppendLine( std::vector<VertexMaster>& vertexes,
                 const LineSeg2D& lineSeg,
                 const Rgba8& tint,
                 float thickness = 1.f );

void AppendRay( std::vector<VertexMaster>& vertexes,
                const LineSeg3D& lineSeg,
                const Rgba8& tint,
                float thickness = 1.f );
void AppendRay( std::vector<VertexMaster>& vertexes,
                const LineSeg3D& lineSeg,
                const Rgba8& startTint,
                const Rgba8& endTint,
                float thickness = 1.f );
void AppendRay( std::vector<VertexMaster>& vertexes,
                std::vector<unsigned int>& indexes,
                const LineSeg3D& lineSeg,
                const Rgba8& tint, float thickness = 1.f );

void AppendArrow( std::vector<VertexMaster>& vertexes,
                  const LineSeg2D& lineSeg,
                  const Rgba8& startTint,
                  const Rgba8& endTint,
                  float thickness = 1.f );
void AppendArrow3D( std::vector<VertexMaster>& vertexes,
                    std::vector<unsigned int>& indexes,
                    const LineSeg3D& lineSeg,
                    const Rgba8& startTint,
                    const Rgba8& endTint,
                    float thickness = 1.f );
void AppendArrow3D( std::vector<VertexMaster>& vertexes,
                    std::vector<unsigned int>& indexes,
                    const LineSeg3D& lineSeg,
                    const Rgba8& tint,
                    float thickness = 1.f );
void AppendCross( std::vector<VertexMaster>& vertexes,
                  const Vec2& center,
                  float height,
                  float thickness,
                  const Rgba8& color,
                  float angleDegrees = 0.f,
                  float aspect = 1.f );

void AppendDisc( std::vector<VertexMaster>& vertexes,
                 const Disc& disc,
                 const Rgba8& tint );
void AppendDiscPerimeter( std::vector<VertexMaster>& vertexes,
                          const Disc& disc,
                          const Rgba8& tint,
                          float thickness );

void AppendAABB2( std::vector<VertexMaster>& vertexes,
                  const AABB2& box,
                  const Rgba8& tint );
void AppendAABB2( std::vector<VertexMaster>& vertexes,
                  const AABB2& box,
                  const Rgba8& tint,
                  const Vec2& uvMins,
                  const Vec2& uvMaxs );
void AppendAABB2Perimeter( std::vector<VertexMaster>& vertexes,
                           const AABB2& box,
                           const Rgba8& tint,
                           float thickness = 1.f );
void AppendAABB3( std::vector<VertexMaster>& vertexes,
                  const AABB3& cube,
                  const Rgba8& tint );

void AppendCylinder( std::vector<VertexMaster>& vertexes,
                     std::vector<unsigned int>& indexes,
                     const LineSeg3D& lineSeg,
                     const Rgba8& startTint,
                     const Rgba8& endTint,
                     float startRadius = 1.f,
                     float endRadius = 1.f,
                     unsigned int sides = 16u );

void AppendCylinderPerimeter( std::vector<VertexMaster>& vertexes,
                              std::vector<unsigned int>& indexes,
                              const LineSeg3D& lineSeg,
                              const Rgba8& tint,
                              float startRadius = 1.f,
                              float endRadius = 1.f,
                              float thickness = 1.f,
                              unsigned int sides = 16u );

void AppendCone( std::vector<VertexMaster>& vertexes,
                 std::vector<unsigned int>& indexes,
                 const LineSeg3D& lineSeg,
                 const Rgba8& startTint,
                 const Rgba8& endTint,
                 float startRadius = 1.f,
                 unsigned int sides = 16u );

void AppendPolygon2D( std::vector<VertexMaster>& vertexes,
                      const std::vector<Vec2>& points,
                      const Rgba8& color );
void AppendPolygon2DPerimeter( std::vector<VertexMaster>& vertexes,
                               const std::vector<Vec2>& points,
                               const Rgba8& color,
                               float thickness = 1.f );
void AppendPlaneSegment( std::vector<VertexMaster>& vertexes,
                         const Mat44& basis,
                         const Rgba8& tint,
                         const Vec2& ijSize = Vec2::ONE,
                         const Vec2& pivot = Vec2::ZERO,
                         const Vec2& uvAtMin = Vec2::ZERO,
                         const Vec2& uvAtMax = Vec2::ONE );
void AppendPlaneSegment( std::vector<VertexMaster>& vertexes,
                         const Vec3& center,
                         const Vec3& scaledIBasis,
                         const Vec3& scaledJBasis,
                         const Rgba8& tint,
                         const Vec2& uvAtMin = Vec2::ZERO,
                         const Vec2& uvAtMax = Vec2::ONE );
