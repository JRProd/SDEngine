#include "MathUtils.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/Capsule2D.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"
#include "Engine/Core/Math/Primatives/OBB2.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"

//-------------------------------------------------------------------------------
// Constants
const float RAD_PER_DEGREES = 180.f / g_PIf;
const float DEGREES_PER_RAD = g_PIf / 180.f;

//-------------------------------------------------------------------------------
// Conversion Functions
float ConvertDegreesToRadians( float degrees )
{
    return degrees * DEGREES_PER_RAD;
}

//-------------------------------------------------------------------------------
float ConvertRadiansToDegrees( float radians )
{
    return radians * RAD_PER_DEGREES;
}

//-------------------------------------------------------------------------------
// Degree Trig Functions
float sinfDegrees( float thetaDegrees )
{
    return sinf( ConvertDegreesToRadians( thetaDegrees ) );
}

//-------------------------------------------------------------------------------
float cosfDegrees( float thetaDegrees )
{
    return cosf( ConvertDegreesToRadians( thetaDegrees ) );
}

float tanfDegrees( float thetaDegrees )
{
    return tanf( ConvertDegreesToRadians( thetaDegrees ) );
}

//-------------------------------------------------------------------------------
float atan2fDegrees( float y, float x )
{
    return ConvertRadiansToDegrees( atan2f( y, x ) );
}

float GetAngleZeroTo360( float currentDegrees )
{
    if ( currentDegrees >= 360 )
    {
        // Performing simple float modulus
        int timesGreater = static_cast<int>(currentDegrees) / 360;
        return currentDegrees - (360.f * timesGreater);
    }
    if ( currentDegrees < 0 )
    {
        if ( currentDegrees < -360.f )
        {
            // Performing simple float modulus
            int timesLesser = static_cast<int>(currentDegrees) / -360;
            float finalDegree = 360.f - (currentDegrees + (360.f * timesLesser));

            return finalDegree;
        }
        return 180.f + (180.f + currentDegrees);
    }
    return currentDegrees;
}

float GetAngleZeroToTwoPI( float currentRadians )
{
    if ( currentRadians >= g_2PIf )
    {
        // Performing simple float modulus
        int timesGreater = static_cast<int>(currentRadians / g_2PIf);
        return currentRadians - (g_2PIf * static_cast<float>(timesGreater));
    }
    if ( currentRadians < 0 )
    {
        if ( currentRadians < -g_2PIf )
        {
            // Performing simple float modulus
            int timesLesser = static_cast<int>(currentRadians / -g_2PIf);
            float finalDegree = g_2PIf - (currentRadians +
                (g_2PIf * static_cast<float>(timesLesser)));

            return finalDegree;
        }
        return g_PIf + (g_PIf + currentRadians);
    }
    return currentRadians;
}

float GetAngleNeg180To180( float currentDegrees )
{
    float zeroTo360 = GetAngleZeroTo360( currentDegrees );
    if ( zeroTo360 > 180.f )
    {
        zeroTo360 -= 360.f;
    }
    return zeroTo360;
}

float GetAngleNegPiToPi( float currentRadians )
{
    float zeroTo2Pi = GetAngleZeroToTwoPI( currentRadians );
    if ( zeroTo2Pi > g_PIf )
    {
        zeroTo2Pi -= g_2PIf;
    }
    return zeroTo2Pi;
}

float GetAngleNegXToX( float currentDegrees, float range )
{
    return Clamp( GetAngleNeg180To180( currentDegrees ), -range, range );
}

float GetShortestAngularDisplacement( float startDegrees, float endDegrees )
{
    float potentialShortest = endDegrees - startDegrees;

    if ( potentialShortest < -180.f )
    {
        // Performing simple float modulus
        int floatMod = static_cast<int>(potentialShortest) / -180;
        float finalMod = potentialShortest + (180.f * floatMod);
        return 180.f + finalMod;
    }
    if ( potentialShortest > 180.f )
    {
        // Performing simple float modulus
        int floatMod = (static_cast<int>(potentialShortest) / 180);
        float finalMod = potentialShortest - (180.f * floatMod);
        if ( potentialShortest < 360.f )
        {
            return finalMod + -180.f;
        }
        return finalMod;
    }
    return potentialShortest;
}

int GetAngleDirectionTowards( float startingAngle, float finishingAngle )
{
    return GetShortestAngularDisplacement( startingAngle, finishingAngle ) > 0 ? 1 : -1;
}

float GetTurnedTowards( float startingAngle, float finishingAngle, float maxAngleDistance )
{
    if ( startingAngle == finishingAngle )
    {
        return finishingAngle;
    }

    float angleDisplacement = GetShortestAngularDisplacement( startingAngle, finishingAngle );
    if ( abs( angleDisplacement ) < maxAngleDistance )
    {
        return finishingAngle;
    }

    if ( angleDisplacement > 0 )
    {
        return startingAngle + maxAngleDistance;
    }
    return startingAngle - maxAngleDistance;
}

bool IsPointInSectorDisc( const Vec2& targetPoint,
                          const Vec2& observerPoint,
                          float forwardDegree,
                          float apertureDegree,
                          float maxRadius )
{
    Vec2 forwardDirection = Vec2::MakeFromPolarDegrees( forwardDegree );
    return IsPointInSectorDisc( targetPoint, observerPoint, forwardDirection, apertureDegree,
                                maxRadius );
}

bool IsPointInSectorDisc( const Vec2& targetPoint,
                          const Vec2& observerPoint,
                          const Vec2& forwardVector,
                          float apertureDegree,
                          float maxRadius )
{
    Vec2 displacement = targetPoint - observerPoint;

    // If the point is outside the 
    if ( displacement.GetLengthSquared() > maxRadius * maxRadius ) { return false; }

    // Get the angle between the forward vector and the displacement to the targetPoint
    float angleBetween = Vec2::GetAngleDegreesBetween( forwardVector, displacement );

    return abs( angleBetween ) < apertureDegree * .5f;
}

//-----------------------------------------------------------------------------
float SmoothStart2( float t )
{
    return t * t;
}

//-----------------------------------------------------------------------------
float SmoothStart3( float t )
{
    return t * t * t;
}

//-----------------------------------------------------------------------------
float SmoothStart4( float t )
{
    return t * t * t * t;
}

//-----------------------------------------------------------------------------
float SmoothStart5( float t )
{
    return t * t * t * t * t;
}

//-----------------------------------------------------------------------------
float SmoothStop2( float t )
{
    float oneMt = 1 - t;
    return 1 - (oneMt * oneMt);
}

//-----------------------------------------------------------------------------
float SmoothStop3( float t )
{
    float oneMt = 1 - t;
    return 1 - (oneMt * oneMt * oneMt);
}

//-----------------------------------------------------------------------------
float SmoothStop4( float t )
{
    float oneMt = 1 - t;
    return 1 - (oneMt * oneMt * oneMt * oneMt);
}

//-----------------------------------------------------------------------------
float SmoothStop5( float t )
{
    float oneMt = 1 - t;
    return 1 - (oneMt * oneMt * oneMt * oneMt * oneMt);
}

//-----------------------------------------------------------------------------
float SmoothStep3( float t )
{
    return (3 * t * t) - (2 * t * t * t);
}

bool DoAABB2sOverlap( const AABB2& box1, const AABB2& box2 )
{
    if ( box1.mins.x > box2.maxs.x || box1.maxs.x < box2.mins.x )
    {
        return false;
    }
    if ( box1.mins.y > box2.maxs.y || box1.maxs.y < box2.mins.y )
    {
        return false;
    }
    return true;
}

bool DoAABB2OverlapCapsule2D( const AABB2& box, const Capsule2D& capsule )
{
    if ( !DoDiscsOverlap( box.GetOuterDisc(), capsule.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 capsulePoints[ 2 ] = {capsule.bone.start, capsule.bone.end};
    if ( box.CheckSimpleSeparationAgainstPoints( 2, capsulePoints, capsule.radius ) )
    {
        return false;
    }

    Vec2 boxPoints[ 4 ];
    box.GetCornerPoints( boxPoints );
    if ( capsule.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    // Check Nearest Axis
    ////////////////////////////////////////////////////////////////////////////////
    // TEMP HACK
    // TODO: Box vs Capsule Collision is incomplete. Temporary fix but not exhaustive.
    ////////////////////////////////////////////////////////////////////////////////
    Vec2 nearestPointOnCapsuleBone = capsule.bone.GetNearestPointOnLineSeg2D( box.GetCenter() );
    Vec2 nearestPointOnBoxFromAbove = box.GetNearestPointInAABB2( nearestPointOnCapsuleBone );
    Vec2 axis = (nearestPointOnCapsuleBone - nearestPointOnBoxFromAbove).GetNormalized();
    FloatRange boxRange = FloatRange::CreateFromPointsOnAxis(
        4, boxPoints, nearestPointOnCapsuleBone, axis );
    FloatRange capsuleRange = FloatRange::CreateFromPointsOnAxis(
        2, capsulePoints, nearestPointOnCapsuleBone, axis, capsule.radius );
    if ( !boxRange.DoesOverlap( capsuleRange ) )
    {
        return false;
    }

    return true;
}

bool DoAABB2OverlapDisc( const AABB2& box, const Disc& disc )
{
    Vec2 discNearestPoint = disc.GetNearestPointInDisc( box.GetCenter() );
    return box.IsPointInside( discNearestPoint );
}

bool DoAABB2OverlapLineSeg2D( const AABB2& box, const LineSeg2D& lineSeg )
{
    if ( !DoDiscsOverlap( box.GetOuterDisc(), lineSeg.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 lineSegPoints[ 2 ] = {lineSeg.start, lineSeg.end};
    if ( box.CheckSimpleSeparationAgainstPoints( 2, lineSegPoints ) )
    {
        return false;
    }

    Vec2 boxPoints[ 4 ];
    box.GetCornerPoints( boxPoints );
    if ( lineSeg.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    return true;
}

bool DoAABB2OverlapOBB2( const AABB2& box1, const OBB2& box2 )
{
    if ( !DoDiscsOverlap( box1.GetOuterDisc(), box2.GetOuterDisc() ) )
    {
        return false;
    }
    if ( DoDiscsOverlap( box1.GetInnerDisc(), box2.GetInnerDisc() ) )
    {
        return true;
    }

    Vec2 boxPoints[ 4 ];
    box1.GetCornerPoints( boxPoints );
    if ( box2.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    box2.GetCornerPoints( boxPoints );
    if ( box1.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    return true;
}

bool DoCapsule2DsOverlap( const Capsule2D& capsule1, const Capsule2D& capsule2 )
{
    UNUSED( capsule1 );
    UNUSED( capsule2 );
    ERROR_AND_DIE( "Function not implemented yet!" );
}

bool DoCapsule2DOverlapDisc( const Capsule2D& capsule, const Disc& disc )
{
    UNUSED( capsule );
    UNUSED( disc );
    ERROR_AND_DIE( "Function not implemented yet!" );
}

bool DoCapsule2DOverlapLineSeg2D( const Capsule2D& capsule, const LineSeg2D& lineSeg )
{
    UNUSED( capsule );
    UNUSED( lineSeg );
    ERROR_AND_DIE( "Function not implemented yet!" );
}

bool DoCapsule2DOverlapOBB2( const Capsule2D& capsule, const OBB2& box )
{
    if ( !DoDiscsOverlap( box.GetOuterDisc(), capsule.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 capsulePoints[ 2 ] = {capsule.bone.start, capsule.bone.end};
    if ( box.CheckSimpleSeparationAgainstPoints( 2, capsulePoints, capsule.radius ) )
    {
        return false;
    }

    Vec2 boxPoints[ 4 ];
    box.GetCornerPoints( boxPoints );
    if ( capsule.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    // Check Nearest Axis
    ////////////////////////////////////////////////////////////////////////////////
    // TEMP HACK
    // TODO: Box vs Capsule Collision is incomplete. Temporary fix but not exhaustive.
    ////////////////////////////////////////////////////////////////////////////////
    Vec2 nearestPointOnCapsuleBone = capsule.bone.GetNearestPointOnLineSeg2D( box.GetCenter() );
    Vec2 nearestPointOnBoxFromAbove = box.GetNearestPoint( nearestPointOnCapsuleBone );
    Vec2 axis = (nearestPointOnCapsuleBone - nearestPointOnBoxFromAbove).GetNormalized();
    FloatRange boxRange = FloatRange::CreateFromPointsOnAxis(
        4, boxPoints, nearestPointOnCapsuleBone, axis );
    FloatRange capsuleRange = FloatRange::CreateFromPointsOnAxis(
        2, capsulePoints, nearestPointOnCapsuleBone, axis, capsule.radius );
    if ( !boxRange.DoesOverlap( capsuleRange ) )
    {
        return false;
    }

    return true;
}

bool DoDiscsOverlap( const Disc& disc1, const Disc& disc2 )
{
    return Vec2::GetDistanceSquared( disc1.center, disc2.center ) < (disc1.radius + disc2.radius) *
        (disc1.radius + disc2.radius);
}

bool DoDiscOverlapLineSeg2D( const Disc& disc, const LineSeg2D& lineSeg )
{
    if ( !DoDiscsOverlap( disc, lineSeg.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 discPoints[ 1 ] = {disc.center};
    if ( lineSeg.CheckSimpleSeparationAgainstPoints( 1, discPoints, disc.radius ) )
    {
        return false;
    }

    return true;
}

bool DoDiscOverlapOBB2( const Disc& disc, const OBB2& box )
{
    if ( !DoDiscsOverlap( disc, box.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 relativeNearestOnBox = box.GetNearestPoint( disc.center ) - disc.center;
    return relativeNearestOnBox.GetLengthSquared() < (disc.radius * disc.radius);
}

bool DoLineSeg2DsOVerlap( const LineSeg2D& lineSeg1, const LineSeg2D& lineSeg2 )
{
    if ( !DoDiscsOverlap( lineSeg1.GetOuterDisc(), lineSeg2.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 linePoints[ 2 ] = {lineSeg2.start, lineSeg2.end};
    if ( lineSeg1.CheckSimpleSeparationAgainstPoints( 2, linePoints ) )
    {
        return false;
    }

    linePoints[ 0 ] = lineSeg1.start;
    linePoints[ 1 ] = lineSeg1.end;
    if ( lineSeg2.CheckSimpleSeparationAgainstPoints( 2, linePoints ) )
    {
        return false;
    }

    return true;
}

bool DoLineSeg2DOverlapOBB2( const LineSeg2D& lineSeg, const OBB2& box )
{
    if ( !DoDiscsOverlap( lineSeg.GetOuterDisc(), box.GetOuterDisc() ) )
    {
        return false;
    }

    Vec2 boxPoints[ 4 ];
    box.GetCornerPoints( boxPoints );
    if ( lineSeg.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    Vec2 linePoints[ 2 ] = {lineSeg.start, lineSeg.end};
    if ( box.CheckSimpleSeparationAgainstPoints( 2, linePoints ) )
    {
        return false;
    }

    return true;
}

bool DoOBB2sOverlap( const OBB2& box1, const OBB2 box2 )
{
    if ( !DoDiscsOverlap( box1.GetOuterDisc(), box2.GetOuterDisc() ) )
    {
        return false;
    }
    if ( DoDiscsOverlap( box1.GetInnerDisc(), box2.GetInnerDisc() ) )
    {
        return true;
    }

    Vec2 boxPoints[ 4 ];
    box1.GetCornerPoints( boxPoints );
    if ( box2.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    box2.GetCornerPoints( boxPoints );
    if ( box1.CheckSimpleSeparationAgainstPoints( 4, boxPoints ) )
    {
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------
bool DoSpheresOverlap( const Vec3& sphere1, float radius1, const Vec3& sphere2, float radius2 )
{
    // Use Squared version to reduce number of computations
    return Vec3::GetDistanceSquared( sphere1, sphere2 ) < (radius1 + radius2) * (radius1 + radius2);
}

//-------------------------------------------------------------------------------
const Vec2 Transform2D( const Vec2& position, float uniformScale, float rotationDegrees,
                        const Vec2& translation )
{
    // Create new basis vectors
    float c = uniformScale * cosfDegrees( rotationDegrees );
    float s = uniformScale * sinfDegrees( rotationDegrees );
    Vec2 i = Vec2( c, s );
    Vec2 j = Vec2( -s, c );

    // Apply the translation
    return Transform2D( position, i, j, translation );
}

const Vec2 Transform2D( const Vec2& position, const Vec2& scale, float rotationDegrees,
                        const Vec2& translation )
{
    // Create new basis vectors
    float c = scale.x * cosfDegrees( rotationDegrees );
    float s = scale.y * sinfDegrees( rotationDegrees );
    Vec2 i = Vec2( c, s );
    Vec2 j = Vec2( -s, c );

    // Apply the translation
    return Transform2D( position, i, j, translation );
}

const Vec2 Transform2D( const Vec2& position,
                        const Vec2& iBasis,
                        const Vec2& jBasis,
                        const Vec2& translation )
{
    // Apply the translation
    return Vec2( position.x * iBasis + position.y * jBasis + translation );
}

//-------------------------------------------------------------------------------
const Vec3 Transform3DXY( const Vec3& position,
                          float uniformScale,
                          float zRotationDegrees,
                          const Vec2& translation )
{
    // Create new basis vectors
    float c = uniformScale * cosfDegrees( zRotationDegrees );
    float s = uniformScale * sinfDegrees( zRotationDegrees );
    Vec2 i = Vec2( c, s );
    Vec2 j = Vec2( -s, c );

    // Apply the translation
    return Transform3DXY( position, i, j, translation );
}

//-------------------------------------------------------------------------------
const Vec3 Transform3DXY( const Vec3& position,
                          const Vec2& scale,
                          float zRotationDegrees,
                          const Vec2& translation )
{
    // Create new basis vectors
    float c = scale.x * cosfDegrees( zRotationDegrees );
    float s = scale.y * sinfDegrees( zRotationDegrees );
    Vec2 i = Vec2( c, s );
    Vec2 j = Vec2( -s, c );

    // Apply the translation
    return Transform3DXY( position, i, j, translation );
}

const Vec3 Transform3DXY( const Vec3& position,
                          const Vec2& iBasis,
                          const Vec2& jBasis,
                          const Vec2& translation )
{
    Vec3 transform = static_cast<Vec3>(Vec2(
        position.x * iBasis + position.y * jBasis + translation ));
    transform.z = position.z;
    return transform;
}

float Clamp( float value, float min, float max )
{
    if ( value < min )
    {
        return min;
    }
    if ( value > max )
    {
        return max;
    }
    return value;
}

float Wrap( float value, float min, float max )
{
    if( value < min)
    {
        return max;
    }
    if( value > max )
    {
        return min;
    }
    return value;
}

float ClampZeroToOne( float value )
{
    return Clamp( value, 0.f, 1.f );
}

float Interpolate( float inputBegin, float inputEnd, float fraction )
{
    return inputBegin + fraction * (inputEnd - inputBegin);
}

float InterpolateAndClamp( float inputBegin, float inputEnd, float fraction )
{
    const float smallest = Minf( inputBegin, inputEnd );
    const float largest = Maxf( inputBegin, inputEnd );
    return Clamp( Interpolate( inputBegin, inputEnd, fraction ), smallest, largest );
}

float RangeMapFloat( float inputBegin,
                     float inputEnd,
                     float outputBegin,
                     float outputEnd,
                     float inputValue )
{
    float inputDisplacement = inputValue - inputBegin;
    float inputRange = inputEnd - inputBegin;
    if ( inputRange == 0 ) { return 0; }

    float pureRange = inputDisplacement / inputRange;
    return Interpolate( outputBegin, outputEnd, pureRange );
}

float RangeMapFloatZeroOne( float inputBegin, float inputEnd, float inputValue )
{
    return RangeMapFloat( inputBegin, inputEnd, 0.f, 1.f, inputValue );
}

Vec2 RangeMapFloatVec2( const float inputBegin, const float inputEnd, const Vec2& outputBegin,
                        const Vec2& outputEnd, const float inputValue )
{
    const float inputDisplacement = inputValue - inputBegin;
    const float inputRange = inputEnd - inputBegin;
    if ( inputRange == 0 ) { return outputBegin; }

    const float pureRange = inputDisplacement / inputRange;
    return Vec2::Lerp( Vec2( pureRange ), outputBegin, outputEnd );
}

int ceili( float value )
{
    return static_cast<int>(ceilf( value ));
}

int roundi( float value )
{
    return static_cast<int>(roundf( value ));
}

int floori( float value )
{
    return static_cast<int>(floorf( value ));
}

bool IsMostlyEqual( const float a, const float b, const float epsilon )
{
    return abs( a - b ) <= epsilon;
}

int Signi( const int a )
{
    return a >= 0 ? 1 : -1;
}

int Signi( const float a )
{
    return a >= 0.f ? 1 : -1;
}

int Signi( const double a )
{
    return a >= 0. ? 1 : -1;
}

float Signf( const int a )
{
    return a >= 0 ? 1.f : -1.f;
}

float Signf( const float a )
{
    return a >= 0.f ? 1.f : -1.f;
}

float Signf( const double a )
{
    return a >= 0 ? 1.f : -1.f;
}

double Sign( const int a )
{
    return a >= 0 ? 1. : -1.;
}

double Sign( const float a )
{
    return a >= 0.f ? 1. : -1.;
}

double Sign( const double a )
{
    return a >= 0. ? 1. : -1.;
}

float Minf( const float a, const float b )
{
    return a < b ? a : b;
}

double Min( const double a, const double b )
{
    return a < b ? a : b;
}

float Maxf( const float a, const float b )
{
    return a > b ? a : b;
}

double Max( const double a, const double b )
{
    return a > b ? a : b;
}

size_t Minu( size_t a, size_t b )
{
    return a < b ? a : b;
}

size_t Maxu( size_t a, size_t b )
{
    return a > b ? a : b;
}

float Minf( const float a, const float b, const float c )
{
    if ( a < b )
    {
        if ( a < c )
        {
            return a;
        }
        return b < c ? b : c;
    }

    return b < c ? b : c;
}

float Maxf( const float a, const float b, const float c )
{
    if ( a > b )
    {
        if ( a > c )
        {
            return a;
        }
        return b > c ? b : c;
    }

    return b > c ? b : c;
}

int absMod( int value, int modulus )
{
    return abs( value ) % abs( modulus );
}
