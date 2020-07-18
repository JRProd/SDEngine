#include "LineSeg2D.hpp"

#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"
#include "Engine/Renderer/DebugRenderer.hpp"

LineSeg2D::LineSeg2D( const LineSeg2D& copy )
    : start( copy.start )
      , end( copy.end )
{
}

LineSeg2D::LineSeg2D( float startX, float startY, float endX, float endY )
    : start( startX, startY )
      , end( endX, endY )
{
}

LineSeg2D::LineSeg2D( const Vec2& startPoint, const Vec2& endPoint )
    : start( startPoint )
      , end( endPoint )
{
}

Vec2 LineSeg2D::GetCenter() const
{
    return start + (GetDisplacement() * .5f);
}

Vec2 LineSeg2D::GetDirection() const
{
    return GetDisplacement().GetNormalized();
}

Vec2 LineSeg2D::GetNormal() const
{
    return GetDirection().GetRotated90Degrees();
}

Vec2 LineSeg2D::GetDisplacement() const
{
    return end - start;
}

float LineSeg2D::GetLength() const
{
    return Vec2::GetDistance( start, end );
}

bool LineSeg2D::GetClippedTo( const LineSeg2D& lineSeg, OUT_PARAM LineSeg2D& outSeg ) const
{
    const Vec2 lineSegDirection = lineSeg.GetDirection();
    const FloatRange lineSegRange( Vec2::Dot( lineSegDirection, lineSeg.start ),
                                   Vec2::Dot( lineSegDirection, lineSeg.end ) );
    const FloatRange myRange( Vec2::Dot( lineSegDirection, start ),
                              Vec2::Dot( lineSegDirection, end ) );
    FloatRange clippedRange = myRange;
    clippedRange.ClipTo( lineSegRange );

    // Line segments do not overlap
    if ( clippedRange.maximum < clippedRange.minimum ) { return false; }


    const Vec2 myDirection = GetDirection();

    outSeg.start = RangeMapFloatVec2( myRange.minimum, myRange.maximum,
                                      start, end, clippedRange.minimum );
    outSeg.end = RangeMapFloatVec2( myRange.minimum, myRange.maximum,
                                    start, end, clippedRange.maximum );
    return true;
}

const LineSeg2D LineSeg2D::CreateFromZeroToPoint( const Vec2& endPoint )
{
    return LineSeg2D( Vec2( 0.f, 0.f ), endPoint );
}

const Disc LineSeg2D::GetOuterDisc() const
{
    const Vec2& displacement = GetDisplacement();
    float lineLength = abs( displacement.x ) + abs( displacement.y );
    return Disc( GetCenter(), lineLength * .5f );
}

const Vec2 LineSeg2D::GetNearestPointOnLineSeg2D( const Vec2& pointToTest ) const
{
    Vec2 lineSegDisplacement = end - start;
    if ( lineSegDisplacement.GetLengthSquared() == 0 ) { return start; }

    Vec2 startToPoint = start - pointToTest;
    Vec2 endToPoint = end - pointToTest;
    float sp = Vec2::Dot( lineSegDisplacement, startToPoint );
    if ( sp > 0 )
    {
        return start;
    }
    if ( Vec2::Dot( lineSegDisplacement, endToPoint ) < 0 )
    {
        return end;
    }
    Vec2 projOntoSegment = startToPoint.GetProjectionOnto( lineSegDisplacement );
    return start - projOntoSegment;
}

float LineSeg2D::GetDistanceToPoint( const Vec2& point ) const
{
    return Vec2::GetDistance( point, GetNearestPointOnLineSeg2D( point ) );
}

bool LineSeg2D::IsOnPositiveSide( const Vec2& point ) const
{
    return Vec2::Dot( point - start, GetNormal() ) > 0;
}

bool LineSeg2D::CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points,
                                                    float radius ) const
{
    const Vec2& center = GetCenter();
    const Vec2& direction = GetDirection();
    float halfBoneLength = GetDisplacement().GetLength() * .5f;
    FloatRange lineSegRange = FloatRange( -halfBoneLength, halfBoneLength );
    FloatRange pointsRange = FloatRange::CreateFromPointsOnAxis(
        numPoints, points, center, direction, radius );
    if ( !lineSegRange.DoesOverlap( pointsRange ) )
    {
        return true;
    }

    // Check Bone Normal Axis
    Vec2 normalDirection = direction.GetRotated90Degrees();
    lineSegRange = FloatRange( 0.f, 0.f );
    pointsRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, center, normalDirection,
                                                      radius );
    if ( !lineSegRange.DoesOverlap( pointsRange ) )
    {
        return true;
    }
    return false;
}
