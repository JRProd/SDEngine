#pragma once

#include "Engine/Core/EngineCommon.hpp"

#include "Vec2.hpp"

struct Disc;

struct LineSeg2D
{
    Vec2 start = Vec2::ZERO;
    Vec2 end = Vec2::ONE;

public:
    //-------------------------------------------------------------------------
    // Static Functions and Memebers
    static const LineSeg2D CreateFromZeroToPoint( const Vec2& endPoint );
    
    static const LineSeg2D UNIT_SEGMENT;
    
    //-------------------------------------------------------------------------
    // Constructors
    LineSeg2D() = default;
    ~LineSeg2D() = default;
    LineSeg2D( const LineSeg2D& copy );
    explicit LineSeg2D( float startX, float startY, float endX, float endY );
    explicit LineSeg2D( const Vec2& startPoint, const Vec2& endPoint );

    //-------------------------------------------------------------------------
    // Accessors
    Vec2 GetCenter() const;
    Vec2 GetDirection() const;
    Vec2 GetNormal() const;
    Vec2 GetDisplacement() const;
    float GetLength() const;
    bool GetClippedTo( const LineSeg2D& lineSeg, OUT_PARAM LineSeg2D& outSeg ) const;

    //-------------------------------------------------------------------------
    // Collision Calculations
    const Disc GetOuterDisc() const;
    const Vec2 GetNearestPointOnLineSeg2D( const Vec2& pointToTest ) const;
    float GetDistanceToPoint( const Vec2& point ) const;
    bool IsOnPositiveSide( const Vec2& point ) const;
    bool CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float radius = 0.f ) const;
};