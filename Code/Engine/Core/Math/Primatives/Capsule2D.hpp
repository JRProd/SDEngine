#pragma once

#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"

struct FloatRange;

struct Capsule2D
{
    LineSeg2D bone;
    float radius;

public:
    //-------------------------------------------------------------------------
    // Static Functions and Members
    static const LineSeg2D UNIT_CAPSULE;

    //-------------------------------------------------------------------------
    // Constructors
    Capsule2D() = default;
    ~Capsule2D() = default;
    Capsule2D( const Capsule2D& copy );
    explicit Capsule2D( float startX, float startY, float endX, float endY, float radiusLength );
    explicit Capsule2D( const Vec2& startPoint, const Vec2& endPoint, float radiusLength );
    explicit Capsule2D( const LineSeg2D& capsuleBone, float radiusLength );

    //-------------------------------------------------------------------------
    // Accessors (const)
    const Vec2 GetCenter() const;
    const Vec2 GetBoneDisplacement() const;
    const Vec2 GetBoneDirection() const;

    //-------------------------------------------------------------------------
    // Collision Functions
    const Disc GetOuterDisc() const;
    bool IsPointInside( const Vec2& pointToTest ) const;
    const Vec2 GetNearestPointOnCapusle2D( const Vec2& pointToTest ) const;
    bool CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float shapeRadius = 0.f ) const;

};