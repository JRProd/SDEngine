#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"

struct Line2D
{
    Vec2 point = Vec2::ZERO;
    Vec2 forward = Vec2::UNIT_EAST;

public:
    Line2D() = default;
    ~Line2D() = default;
    Line2D( const Line2D& copy );

    explicit Line2D( float pointAX, float pointAY, float pointBX, float pointBY );
    explicit Line2D( float pointX, float pointY, float angleDegrees );
    explicit Line2D( const Vec2& pointA, const Vec2& forwardVector );

    static const Line2D HORIZONTAL_AROUND_ZERO;
    static const Line2D VERTICAL_AROUND_ZERO;

    static const Line2D CreateFromPointAndAngle( const Vec2& point, float angleDegrees );
    static const Line2D CreateHorizontalAroundPoint( const Vec2& point );
    static const Line2D CreateVerticalAroundPoint( const Vec2& point );

    bool DoesPointOverlap( const Vec2& pointToTest ) const;
    const Vec2 GetNearestPointOnLineSeg2D( const Vec2& pointToTest ) const;
};