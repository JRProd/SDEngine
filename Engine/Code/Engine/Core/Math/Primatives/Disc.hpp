#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"

struct AABB2;

struct Disc
{
    Vec2 center = Vec2::ZERO;
    float radius = 0.f;

public:
    Disc() {}
    ~Disc() {}
    Disc( const Disc& copy );
    explicit Disc( const Vec2& centerPoint, float radiusOfCircle );
    explicit Disc( const Vec2& pointOne, const Vec2& pointTwo );
    explicit Disc( const Vec2& pointOne, const Vec2& pointTwo, const Vec2& pointThree );

    static Disc UNIT_DISC;

    static bool Intersects( const Disc& disc1, const Disc& disc2 );
    static void PushDiscMobileOutOfDiscMobile( Disc& mobile1, Disc& mobile2 );
    static void PushDiscMobileOutOfDiscFixed( Disc& mobile, const Disc& fixed );

    //-------------------------------------------------------------------------
    // Utility Functions (const)
    const Disc GetTranslation( const Vec2& translation ) const;
    const Disc GetAddedRadiusLength( float addLength ) const;
    const Disc GetScaledRadiusLength( float radiusScale ) const;

    const Vec2 GetNearestPointInDisc( const Vec2& point ) const;

    //-------------------------------------------------------------------------
    // Utility Functions (non-const)
    void Translate( const Vec2& translation );
    void AddRadiusLength( float addLength );
    void ScaleRadiusLength( float radiusScale );

    //-------------------------------------------------------------------------
    // Operators (const)
    const bool operator==( const Disc& compare ) const;
    const bool operator!=( const Disc& compare ) const;

    //-------------------------------------------------------------------------
    // Operators (self-mutators / non-const)
    void operator=( const Disc& copyFrom );

    //-------------------------------------------------------------------------
    // Collisions
    bool IsPointInside( const Vec2& point ) const;
    void PushOutOfPointFixed( const Vec2& point );

    void PushOutOfAABB2Fixed( const AABB2& fixedBox );

};

//-----------------------------------------------------------------------------
// Math Unit Test Functions
bool DoDiscsOverlap( const Vec2& disc1,
                     float radius1,
                     const Vec2& disc2,
                     float radius2 );


const Vec2 GetNearestPointOnDisc( const Vec2& referencePosition,
                                  const Vec2& discCeter,
                                  float discRadius );
void PushDiscOutOfPoint( Vec2& discCenter,
                         float discRadius,
                         const Vec2& fixedPoint );
void PushDiscOutOfDisc( Vec2& mobileCenter,
                        float mobileRadius,
                        const Vec2& fixedCenter,
                        float fixedRadius );
void PushDiscsOutOfEachOther( Vec2& mobileCenter1, float mobileRadius1,
                              Vec2& mobileCenter2, float mobileRadius2 );
void PushDiscOutOfAABB2( Vec2& mobileCenter,
                         float mobileRadius,
                         const AABB2& fixedBox );
