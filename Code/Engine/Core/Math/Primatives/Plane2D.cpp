#include "Plane2D.hpp"

#include "Engine/Core/Math/MathUtils.hpp"

Plane2D::Plane2D( const Vec2& direction, const float dist )
    : normal( direction )
      , distance( dist )
{
}

Plane2D::Plane2D( const Vec2& direction, const Vec2& point )
    : normal( direction )
      , distance( Vec2::Dot( normal, point ) )
{
}

bool Plane2D::IsOnPositiveSide( const Vec2& point ) const
{
    return Vec2::Dot( point, normal ) > distance;
}

bool Plane2D::IsOnPlane( const Vec2& point ) const
{
    return IsMostlyEqual( Vec2::Dot( point, normal ), distance, .01f );
}

Vec2 Plane2D::GetTangent() const
{
    return normal.GetRotated90Degrees();
}

LineSeg2D Plane2D::GetClippedEdge( const LineSeg2D& lineSeg ) const
{
    const float dotStart = Vec2::Dot( normal, lineSeg.start );
    const float dotEnd = Vec2::Dot( normal, lineSeg.end );

    Vec2 newStart;
    Vec2 newEnd;
    if ( dotStart >= 0.f )
    {
        newStart = lineSeg.start;
    }

    if ( dotEnd >= 0.f )
    {
        newEnd = lineSeg.end;
    }

    if ( dotStart * dotEnd < 0.f )
    {
        const Vec2 displacement = lineSeg.GetDisplacement();
        const float alongLine = dotStart / (dotStart - dotEnd);
        const Vec2 newPoint = displacement * alongLine + lineSeg.start;

        if ( dotStart < 0 )
        {
            newStart = newPoint;
        }
        else
        {
            newEnd = newPoint;
        }
    }
    else
    {
        newStart = normal * distance;
        newEnd = normal * distance;
    }

    return LineSeg2D( newStart, newEnd );
}
