#include "Line2D.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"

Line2D::Line2D( const Line2D& copy )
    : point( copy.point )
    , forward( copy.forward )
{
}

Line2D::Line2D( float pointAX, float pointAY, float pointBX, float pointBY )
    : point( pointAX, pointAY )
    , forward( Vec2( pointBY - pointAY, pointBX - pointAX ).GetNormalized() )
{
}

Line2D::Line2D( float pointX, float pointY, float angleDegrees )
    : point( pointX, pointY )
    , forward( Vec2::MakeFromPolarDegrees( angleDegrees ) )
{
}

Line2D::Line2D( const Vec2& pointA, const Vec2& forwardVector )
    : point( pointA )
    , forward( forwardVector.GetNormalized() )
{
}

STATIC const Line2D Line2D::CreateFromPointAndAngle( const Vec2& point, float angleDegrees )
{
    return Line2D( point, Vec2::MakeFromPolarDegrees( angleDegrees ) );
}

STATIC const Line2D Line2D::CreateHorizontalAroundPoint( const Vec2& point )
{
    return Line2D( point, Vec2::UNIT_EAST );
}

STATIC const Line2D Line2D::CreateVerticalAroundPoint( const Vec2& point )
{
    return Line2D( point, Vec2::UNIT_NORTH );
}

bool Line2D::DoesPointOverlap( const Vec2& pointToTest ) const
{
    return GetNearestPointOnLineSeg2D( pointToTest ) == pointToTest;
}

const Vec2 Line2D::GetNearestPointOnLineSeg2D( const Vec2& pointToTest ) const
{
    return point + pointToTest.GetProjectionOnto( forward );
}

STATIC const Line2D Line2D::HORIZONTAL_AROUND_ZERO = Line2D( Vec2::ZERO, Vec2::UNIT_EAST );
STATIC const Line2D Line2D::VERTICAL_AROUND_ZERO = Line2D( Vec2::ZERO, Vec2::UNIT_NORTH );