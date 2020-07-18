#pragma once

#include "Engine/Core/Math/Primatives/AABB2.hpp"

struct OBB2
{
    Vec2 center = Vec2::ZERO;
    Vec2 halfDimension = Vec2::ZERO;
    Vec2 iBasis = Vec2::ZERO;

public:
    //-------------------------------------------------------------------------
    // Constructors
    ~OBB2() = default;
    OBB2() = default;
    OBB2( const OBB2& copyFrom );
    explicit OBB2( const Vec2& boxCenter, const Vec2& fullDimension, const Vec2& iBasisNormalized = Vec2::UNIT_EAST );
    explicit OBB2( const Vec2& boxCenter, const Vec2& fullDimension, float angleDegrees );
    explicit OBB2( const AABB2& aabb2, float angleDegrees = 0.f );

    //-------------------------------------------------------------------------
    // Static Methonds and Variables

    //-------------------------------------------------------------------------
    // Accessors (const)
    const Vec2 GetCenter() const { return center; }
    const Vec2 GetDimension() const { return halfDimension * 2.f; }
    const Vec2 GetIBasis() const { return iBasis; }
    const Vec2 GetJBasis() const { return Vec2( -iBasis.y, iBasis.x ); }
    float GetOrientation() const { return iBasis.GetAngleDegrees(); }
    const Vec2 GetPointAtUV( const Vec2& uvPoint ) const;
    const Vec2 GetUVForPoint( const Vec2& point ) const;
    void GetCornerPoints( Vec2* output ) const; // Top Right, Top Left, Bottom Right, Bottom Left

    //-------------------------------------------------------------------------
    // Mutators (non-const)
    void SetCenter( const Vec2& newCenter );
    void Translate( const Vec2& translation );
    void SetDimension( const Vec2& newDimension );
    void AddDimension( const Vec2& deltaDimension );
    void SetOrientationDegrees( float newDegrees );
    void RotateDegrees( float deltaDegrees );

    //-------------------------------------------------------------------------
    // Collision Functions
    const Disc GetInnerDisc() const;
    const Disc GetOuterDisc() const;
    bool IsPointInside( const Vec2& testPoint ) const;
    const Vec2 GetNearestPoint( const Vec2& testPoint ) const;
    bool CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float radius = 0.f ) const;

    //-------------------------------------------------------------------------
    // Utilities
    // Fixes some requirements of OBB2
    //  1) halfDimension is always non-negative
    //  2) iBasis is always normalized
    void Fix();

private:
    // Get the point in local space relative to the center
    const Vec2 GetPointInLocalSpace( const Vec2& pointInWorld ) const;
    // Gets the point in world space that was relative to the center
    const Vec2 GetPointInWorldSpace( const Vec2& pointInLocal ) const;
};