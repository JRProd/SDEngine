#include "Engine/Core/Utils/VectorPcuUtils.hpp"

#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/Capsule2D.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"
#include "Engine/Core/Math/Primatives/OBB2.hpp"

void AppendCrossToVectorMaster( std::vector<VertexMaster>& vector,
                                const Vec2& center, float height,
                                float thickness,
                                const Rgba8& color,
                                float angleDegrees,
                                float aspect )
{
    OBB2 crossBounds = OBB2( center, Vec2( height * aspect, height ),
                             Vec2::MakeFromPolarDegrees( angleDegrees ) );
    Vec2 boxPoints[ 4 ];
    crossBounds.GetCornerPoints( boxPoints );

    AppendLineSeg2DToVectorMaster( vector,
                                   LineSeg2D( boxPoints[ 2 ], boxPoints[ 0 ] ),
                                   color,
                                   thickness );
    AppendLineSeg2DToVectorMaster( vector,
                                   LineSeg2D( boxPoints[ 1 ], boxPoints[ 3 ] ),
                                   color,
                                   thickness );
}
//
// void AppendAABB2ToVectorMaster( std::vector<VertexMaster>& vector,
//                                 const AABB2& aabb2,
//                                 const Rgba8& color,
//                                 const Vec2& uvAtTopLeft,
//                                 const Vec2& uvAtBottomRight )
// {
//     Vec2 topLeft = Vec2( aabb2.mins.x, aabb2.maxs.y );
//     Vec2 bottomRight = Vec2( aabb2.maxs.x, aabb2.mins.y );
//
//     vector.emplace_back( aabb2.mins, color, Vec2( uvAtTopLeft.x, uvAtBottomRight.y ) );
//     vector.emplace_back( aabb2.maxs, color, Vec2( uvAtBottomRight.x, uvAtTopLeft.y ) );
//     vector.emplace_back( topLeft, color, Vec2( uvAtTopLeft.x, uvAtTopLeft.y ) );
//
//     vector.emplace_back( aabb2.mins, color, Vec2( uvAtTopLeft.x, uvAtBottomRight.y ) );
//     vector.emplace_back( bottomRight, color, Vec2( uvAtBottomRight.x, uvAtBottomRight.y ) );
//     vector.emplace_back( aabb2.maxs, color, Vec2( uvAtBottomRight.x, uvAtTopLeft.y ) );
// }

void AppendCapsule2DToVectorMaster( std::vector<VertexMaster>& vector,
                                    const Capsule2D& capsule,
                                    const Rgba8& color )
{
    // Draw OBB2 at bone
    Vec2 fullDimensions = Vec2( capsule.GetBoneDisplacement().GetLength(), capsule.radius * 2.f );
    OBB2 centerBox = OBB2( capsule.GetCenter(), fullDimensions, capsule.GetBoneDirection() );
    AppendOBB2ToVectorMaster( vector, centerBox, color );

    // Draw First SectorTop
    Disc bottom = Disc( capsule.bone.start, capsule.radius );
    float angle = capsule.GetBoneDirection().GetAngleDegrees();
    AppendSector2DToVectorMaster( vector, bottom, 180.f, angle + 180.f, color );

    // Draw Second SectorTop
    Disc top = Disc( capsule.bone.end, capsule.radius );
    AppendSector2DToVectorMaster( vector, top, 180.f, angle, color );
}

constexpr int CIRCLE_RADIUSES = 64;

void AppendDiscToVectorMaster( std::vector<VertexMaster>& vector, const Disc& disc,
                               const Rgba8& color )
{
    float degreesPerRadius = 360.f / CIRCLE_RADIUSES;

    Vec2 currRadius = Vec2::MakeFromPolarDegrees( 0.f, disc.radius ) + disc.center;
    for ( int currentRadius = 1; currentRadius <= CIRCLE_RADIUSES; ++currentRadius )
    {
        float currentDegress = (degreesPerRadius * currentRadius);
        Vec2 nextRadious = Vec2::MakeFromPolarDegrees( currentDegress, disc.radius ) + disc.center;

        vector.emplace_back( disc.center, color );
        vector.emplace_back( currRadius, color );
        vector.emplace_back( nextRadious, color );

        currRadius = nextRadious;
    }
}

void AppendDiscPerimeterToVectorMaster( std::vector<VertexMaster>& vector,
                                        const Disc& disc,
                                        const Rgba8& color,
                                        float thickness )
{
    float degreesPerRadius = 360.f / CIRCLE_RADIUSES;
    float lineRadius = thickness * .5f;
    Vec2 currShort = Vec2::MakeFromPolarDegrees( 0.f,
                                                 disc.radius - lineRadius ) + disc.center;
    Vec2 currLong = Vec2::MakeFromPolarDegrees( 0.f,
                                                disc.radius + lineRadius ) + disc.center;
    // Start at the second radius because current is defined above.
    //  Loop up to <= CIRCLE_RADIUSES because 360 = 0 degrees
    //  This finishes the last group of triangles
    for ( int currentRadius = 1; currentRadius <= CIRCLE_RADIUSES; ++currentRadius )
    {
        float currentDegrees = degreesPerRadius * currentRadius;
        Vec2 nextShort = Vec2::MakeFromPolarDegrees( currentDegrees,
                                                     disc.radius - lineRadius ) + disc.center;
        Vec2 nextLong = Vec2::MakeFromPolarDegrees( currentDegrees,
                                                    disc.radius + lineRadius ) + disc.center;

        // First triangle
        vector.emplace_back( currShort, color );
        vector.emplace_back( currLong, color );
        vector.emplace_back( nextShort, color );
        // Second Triangle
        vector.emplace_back( nextShort, color );
        vector.emplace_back( currLong, color );
        vector.emplace_back( nextLong, color );

        currShort = nextShort;
        currLong = nextLong;
    }
}

void AppendLineSeg2DToVectorMaster( std::vector<VertexMaster>& vector,
                                    const LineSeg2D& lineSeg,
                                    const Rgba8& color,
                                    float thickness )
{
    Vec2 displacement = lineSeg.GetDisplacement();
    float lineRadius = thickness * .5f;

    Vec2 forward = lineRadius * displacement.GetNormalized();
    Vec2 left = forward.GetRotated90Degrees();

    vector.emplace_back( lineSeg.start - forward - left, color );
    vector.emplace_back( lineSeg.end + forward - left, color );
    vector.emplace_back( lineSeg.end + forward + left, color );

    vector.emplace_back( lineSeg.end + forward + left, color );
    vector.emplace_back( lineSeg.start - forward + left, color );
    vector.emplace_back( lineSeg.start - forward - left, color );
}

void AppendSector2DToVectorMaster( std::vector<VertexMaster>& vector, const Disc& disc,
                                   float aperature, float angle, const Rgba8& color )
{
    int radiuses = ceili( static_cast<float>(CIRCLE_RADIUSES) * (aperature / 360.f) );
    float degreesPerRadius = aperature / radiuses;
    float startingDegree = angle - (aperature * .5f);

    Vec2 currRadius = Vec2::MakeFromPolarDegrees( startingDegree, disc.radius ) + disc.center;
    for ( int currentRadius = 1; currentRadius <= radiuses; ++currentRadius )
    {
        float currentDegress = startingDegree + (degreesPerRadius * currentRadius);
        Vec2 nextRadious = Vec2::MakeFromPolarDegrees( currentDegress, disc.radius ) + disc.center;

        vector.emplace_back( disc.center, color );
        vector.emplace_back( currRadius, color );
        vector.emplace_back( nextRadious, color );

        currRadius = nextRadious;
    }
}

void AppendOBB2ToVectorMaster( std::vector<VertexMaster>& vector,
                               const OBB2& obb2,
                               const Rgba8& color,
                               const Vec2& uvAtMin,
                               const Vec2& uvAtMax )
{
    Vec2 boxPoints[ 4 ];
    obb2.GetCornerPoints( boxPoints );

    vector.emplace_back( boxPoints[ 2 ], color, Vec2( uvAtMin.x, uvAtMin.y ) );
    vector.emplace_back( boxPoints[ 3 ], color, Vec2( uvAtMax.x, uvAtMax.y ) );
    vector.emplace_back( boxPoints[ 0 ], color, Vec2( uvAtMin.x, uvAtMax.y ) );

    vector.emplace_back( boxPoints[ 2 ], color, Vec2( uvAtMin.x, uvAtMin.y ) );
    vector.emplace_back( boxPoints[ 0 ], color, Vec2( uvAtMax.x, uvAtMin.y ) );
    vector.emplace_back( boxPoints[ 1 ], color, Vec2( uvAtMax.x, uvAtMax.y ) );
}
