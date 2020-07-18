#include "Disc.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/AABB2.hpp"

Disc::Disc( const Disc& copy )
    : center( copy.center )
      , radius( copy.radius )
{
}

Disc::Disc( const Vec2& centerPoint, float radiusOfCircle )
    : center( centerPoint )
      , radius( radiusOfCircle )
{
}

Disc::Disc( const Vec2& pointOne, const Vec2& pointTwo )
{
    center = pointOne * .5f + pointTwo * .5f;
    radius = Vec2::GetDistance( pointOne, pointTwo ) * .5f;
}

Disc::Disc( const Vec2& pointOne, const Vec2& pointTwo, const Vec2& pointThree )
{
    // Algorithm from https://en.wikipedia.org/wiki/Circumscribed_circle
    float oneDot = Vec2::Dot( pointOne, pointOne );
    float twoDot = Vec2::Dot( pointTwo, pointTwo );
    float threeDot = Vec2::Dot( pointThree, pointThree );

    float d = 2.f * (pointOne.x * (pointTwo.y - pointThree.y) +
        pointTwo.x * (pointThree.y - pointOne.y) +
        pointThree.x * (pointOne.y - pointTwo.y));

    center.x = (oneDot * (pointTwo.y - pointThree.y) +
        twoDot * (pointThree.y - pointOne.y) +
        threeDot * (pointOne.y - pointTwo.y)) / d;
    center.y = (oneDot * (pointThree.x - pointTwo.x) +
        twoDot * (pointOne.x - pointThree.x) +
        threeDot * (pointTwo.x - pointOne.x)) / d;

    radius = Vec2::GetDistance( center, pointOne );
}

STATIC bool Disc::Intersects( const Disc& disc1, const Disc& disc2 )
{
    float radiusSquared = (disc1.radius + disc2.radius);
    radiusSquared *= radiusSquared;

    return Vec2::GetDistanceSquared( disc1.center, disc2.center ) < radiusSquared;
}

STATIC void Disc::PushDiscMobileOutOfDiscMobile( Disc& mobile1, Disc& mobile2 )
{
    Vec2 displacement = mobile2.center - mobile1.center;
    float halfOverlap = ((mobile1.radius + mobile2.radius) - displacement.GetLength()) * .5f;

    // If discs do not overlap don't do anything
    if ( halfOverlap < 0 ) { return; }

    displacement.Normalize();
    displacement *= halfOverlap;

    mobile1.center -= displacement;
    mobile2.center += displacement;
}

STATIC void Disc::PushDiscMobileOutOfDiscFixed( Disc& mobile, const Disc& fixed )
{
    Vec2 displacement = mobile.center - fixed.center;
    float overlap = ((mobile.radius + fixed.radius) - displacement.GetLength());

    // If discs do not overlap don't do anything
    if ( overlap < 0 ) { return; }

    displacement.Normalize();
    displacement *= overlap;

    mobile.center += displacement;
}

const Disc Disc::GetTranslation( const Vec2& translation ) const
{
    return Disc( center + translation, radius );
}

const Disc Disc::GetAddedRadiusLength( float addLength ) const
{
    return Disc( center, radius + addLength );
}

const Disc Disc::GetScaledRadiusLength( float radiusScale ) const
{
    return Disc( center, radius * radiusScale );
}

const Vec2 Disc::GetNearestPointInDisc( const Vec2& point ) const
{
    Vec2 displacement = point - center;
    displacement.ClampLength( radius );
    return center + displacement;
}

void Disc::Translate( const Vec2& translation )
{
    center += translation;
}

void Disc::AddRadiusLength( float addLength )
{
    radius += addLength;
}

void Disc::ScaleRadiusLength( float radiusScale )
{
    radius *= radiusScale;
}

const bool Disc::operator==( const Disc& compare ) const
{
    return center == compare.center && radius == compare.radius;
}

const bool Disc::operator!=( const Disc& compare ) const
{
    return center != compare.center || radius != compare.radius;
}

void Disc::operator=( const Disc& copyFrom )
{
    center = copyFrom.center;
    radius = copyFrom.radius;
}

bool Disc::IsPointInside( const Vec2& point ) const
{
    return Vec2::GetDistanceSquared( center, point ) <= radius * radius + EPSILON_MULTIPLY( radius * radius );
}

void Disc::PushOutOfPointFixed( const Vec2& point )
{
    Vec2 displacement = center - point;
    float overlap = radius - displacement.GetLength();

    // If point is not inside disc don't do anything
    if ( overlap < 0 ) { return; }

    displacement.Normalize();
    displacement *= overlap;

    center += displacement;
}

void Disc::PushOutOfAABB2Fixed( const AABB2& fixedBox )
{
    Vec2 closestPoint = fixedBox.GetNearestPointInAABB2( center );

    // If the center is inside the fixed box, move away from the boxes center
    if ( closestPoint == center )
    {
        float angleToBoxCenter = (fixedBox.GetCenter() - center).GetAngleDegrees();
        closestPoint += Vec2::MakeFromPolarDegrees( angleToBoxCenter, .00005f );
    }

    PushOutOfPointFixed( closestPoint );
}

STATIC Disc Disc::UNIT_DISC = Disc( Vec2( 0.f, 0.f ), 1.f );

//-------------------------------------------------------------------------------
// Depreciated, should use the disc struct and their DoDiscOverlap method
bool DoDiscsOverlap( const Vec2& disc1, float radius1, const Vec2& disc2, float radius2 )
{
    // Use squared version to reduce number of computations
    return Vec2::GetDistanceSquared( disc1, disc2 ) < (radius1 + radius2) * (radius1 + radius2);
}

const Vec2 GetNearestPointOnDisc( const Vec2& referencePosition, const Vec2& discCeter,
                                  float discRadius )
{
    return Disc( discCeter, discRadius ).GetNearestPointInDisc( referencePosition );
}

void PushDiscOutOfPoint( Vec2& discCenter, float discRadius, const Vec2& fixedPoint )
{
    Disc disc( discCenter, discRadius );
    disc.PushOutOfPointFixed( fixedPoint );
    discCenter = disc.center;
}

void PushDiscOutOfDisc( Vec2& mobileCenter, float mobileRadius, const Vec2& fixedCenter,
                        float fixedRadius )
{
    Disc mobile( mobileCenter, mobileRadius );
    Disc fixed( fixedCenter, fixedRadius );
    Disc::PushDiscMobileOutOfDiscFixed( mobile, fixed );
    mobileCenter = mobile.center;
}

void PushDiscsOutOfEachOther( Vec2& mobileCenter1, float mobileRadius1, Vec2& mobileCenter2,
                              float mobileRadius2 )
{
    Disc mobile1( mobileCenter1, mobileRadius1 );
    Disc mobile2( mobileCenter2, mobileRadius2 );
    Disc::PushDiscMobileOutOfDiscMobile( mobile1, mobile2 );
    mobileCenter1 = mobile1.center;
    mobileCenter2 = mobile2.center;
}

void PushDiscOutOfAABB2( Vec2& mobileCenter, float mobileRadius, const AABB2& fixedBox )
{
    Disc mobile( mobileCenter, mobileRadius );
    mobile.PushOutOfAABB2Fixed( fixedBox );
    mobileCenter = mobile.center;
}
