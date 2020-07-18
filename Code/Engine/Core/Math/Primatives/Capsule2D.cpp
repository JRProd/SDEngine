#include "Capsule2D.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"

Capsule2D::Capsule2D( const Capsule2D& copy )
    : bone( copy.bone )
    , radius( copy.radius )
{
}

Capsule2D::Capsule2D( float startX, float startY, float endX, float endY, float radiusLength )
    : bone( startX, startY, endX, endY )
    , radius( radiusLength )
{
}

Capsule2D::Capsule2D( const Vec2& startPoint, const Vec2& endPoint, float radiusLength )
    : bone( startPoint, endPoint )
    , radius( radiusLength )
{
}

Capsule2D::Capsule2D( const LineSeg2D& capsuleBone, float radiusLength )
    : bone( capsuleBone )
    , radius( radiusLength )
{
}

const Vec2 Capsule2D::GetCenter() const
{
    return bone.GetCenter();
}

const Vec2 Capsule2D::GetBoneDisplacement() const
{
    return bone.GetDisplacement();
}

const Vec2 Capsule2D::GetBoneDirection() const
{
    return bone.GetDirection();
}

const Disc Capsule2D::GetOuterDisc() const
{
    Disc boneDisc = bone.GetOuterDisc();
    boneDisc.radius += radius;
    return boneDisc;
}

bool Capsule2D::IsPointInside( const Vec2& pointToTest ) const
{
    ////////////////////////////////////////////////////////////////////////////////
    // TEMP HACK
    // TODO: Possibly bad, this is a quick solution
    return GetNearestPointOnCapusle2D( pointToTest ) == pointToTest;
    ////////////////////////////////////////////////////////////////////////////////
}

const Vec2 Capsule2D::GetNearestPointOnCapusle2D( const Vec2& pointToTest ) const
{
    Vec2 boneDisplacement = GetBoneDisplacement();
    Vec2 startToPoint = pointToTest - bone.start;
    Vec2 endToPoint = pointToTest - bone.end;
    float sp = Vec2::Dot( boneDisplacement, startToPoint );
    if ( sp < 0 )
    {
        return Disc( bone.start, radius ).GetNearestPointInDisc( pointToTest );
    }
    else if ( Vec2::Dot( boneDisplacement, endToPoint ) > 0 )
    {
        return Disc( bone.end, radius ).GetNearestPointInDisc( pointToTest );
    }
    else
    {
        Vec2 projOnSegment = startToPoint.GetProjectionOnto( boneDisplacement );
        Vec2 pointOnBoneToLocalPoint = startToPoint  - projOnSegment;
        pointOnBoneToLocalPoint.ClampLength( radius );
        projOnSegment += pointOnBoneToLocalPoint;
        return projOnSegment + bone.start;
    }
}

bool Capsule2D::CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float shapeRadius ) const
{
    float halfBoneLength = GetBoneDisplacement().GetLength() * .5f;
    Vec2 boneDirection = GetBoneDirection();
    FloatRange capsuleRange = FloatRange( -halfBoneLength - radius, halfBoneLength + radius );
    FloatRange pointsRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, GetCenter(), boneDirection, shapeRadius );
    if ( !capsuleRange.DoesOverlap( pointsRange ) )
    {
        return true;
    }

    // Check Bone Normal Axis
    Vec2 capsuleTangent = boneDirection.GetRotated90Degrees();
    capsuleRange = FloatRange( -radius, radius );
    pointsRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, GetCenter(), capsuleTangent, shapeRadius );
    if ( !capsuleRange.DoesOverlap( pointsRange ) )
    {
        return true;
    }

    return false;
}
