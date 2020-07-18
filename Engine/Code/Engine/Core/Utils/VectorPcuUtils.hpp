#pragma once

#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"

#include <vector>

struct AABB2;
struct Capsule2D;
struct Disc;
struct LineSeg2D;
struct OBB2;

void AppendCrossToVectorMaster( std::vector<VertexMaster>& vector,
                             const Vec2& center,
                             float height,
                             float thickness,
                             const Rgba8& color,
                             float angleDegrees = 0.f,
                             float aspect = 1.f );

// void AppendAABB2ToVectorMaster( std::vector<VertexMaster>& vector,
//                              const AABB2& aabb2,
//                              const Rgba8& color,
//                              const Vec2& uvAtMin = Vec2::ZERO,
//                              const Vec2& uvAtMax = Vec2::ONE );

void AppendCapsule2DToVectorMaster( std::vector<VertexMaster>& vector,
                                 const Capsule2D& capsule,
                                 const Rgba8& color );

void AppendDiscToVectorMaster( std::vector<VertexMaster>& vector,
                                     const Disc& disc,
                                     const Rgba8& color );

void AppendDiscPerimeterToVectorMaster( std::vector<VertexMaster>& vector,
                                     const Disc& disc,
                                     const Rgba8& color,
                                     float thickness = 1.f );

void AppendLineSeg2DToVectorMaster( std::vector<VertexMaster>& vector,
                                 const LineSeg2D& lineSeg,
                                 const Rgba8& color,
                                 float thickness = 1.f );

void AppendSector2DToVectorMaster( std::vector<VertexMaster>& vector,
                                const Disc& position,
                                float aperature,
                                float angle,
                                const Rgba8& color );

void AppendOBB2ToVectorMaster( std::vector<VertexMaster>& vector,
                            const OBB2& obb2,
                            const Rgba8& color,
                            const Vec2& uvAtMin = Vec2::ZERO,
                            const Vec2& uvAtMax = Vec2::ONE );