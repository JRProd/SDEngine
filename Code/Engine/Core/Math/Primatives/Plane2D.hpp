#pragma once
#include "LineSeg2D.hpp"
#include "Vec2.hpp"

struct Plane2D
{
    Vec2 normal;
    float distance = 0.f;

public:
    Plane2D() = default;
    Plane2D( const Vec2& direction, float dist );
    Plane2D( const Vec2& direction, const Vec2& point );

    bool IsOnPositiveSide( const Vec2& point ) const;
    bool IsOnPlane( const Vec2& point ) const;
    Vec2 GetTangent() const;
    LineSeg2D GetClippedEdge( const LineSeg2D& lineSeg ) const;
};
