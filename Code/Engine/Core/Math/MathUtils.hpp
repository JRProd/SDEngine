#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define EPSILON_MULTIPLY(r) ((1e-14) * (r))

constexpr float g_PIf = static_cast<float>(M_PI);
constexpr float g_2PIf = static_cast<float>(2. * M_PI);
constexpr float g_ROOT_2_OVER_2 = 0.7071068f;

struct AABB2;
struct Capsule2D;
struct Disc;
struct LineSeg2D;
struct OBB2;
struct Vec2;
struct Vec3;
struct FloatRange;

//-------------------------------------------------------------------------------
// Conversions
float ConvertDegreesToRadians( float degrees );
float ConvertRadiansToDegrees( float radians );

//-------------------------------------------------------------------------------
// Trigonometric Functions
float sinfDegrees( float thetaDegrees );
float cosfDegrees( float thetaDegrees );
float tanfDegrees( float thetaDegrees );
float atan2fDegrees( float y, float x );

//-----------------------------------------------------------------------------
// Angle Functions
float GetAngleZeroTo360( float currentDegrees );
float GetAngleZeroToTwoPI( float currentRadians );
float GetAngleNeg180To180( float currentDegrees );
float GetAngleNegPiToPi( float currentRadians );
float GetAngleNegXToX( float currentDegrees, float range );
float GetShortestAngularDisplacement( float startDegrees, float endDegrees );
int GetAngleDirectionTowards( float startingAngle,
                              float finishingAngle );
float GetTurnedTowards( float startingAngle,
                        float finishingAngle,
                        float maxAngleDisplacement );
bool IsPointInSectorDisc( const Vec2& targetPoint,
                          const Vec2& observerPoint,
                          float forwardDegree,
                          float apertureDegree,
                          float maxRadius );
bool IsPointInSectorDisc( const Vec2& targetPoint,
                          const Vec2& observerPoint,
                          const Vec2& forwardVector,
                          float apertureDegree,
                          float maxRadius );

//-------------------------------------------------------------------------------
// Easing Functions
float SmoothStart2( float t );
float SmoothStart3( float t );
float SmoothStart4( float t );
float SmoothStart5( float t );
float SmoothStop2( float t );
float SmoothStop3( float t );
float SmoothStop4( float t );
float SmoothStop5( float t );
float SmoothStep3( float t );

//-------------------------------------------------------------------------------
// Collision Detection Functions
bool DoAABB2sOverlap( const AABB2& box1, const AABB2& box2 );
bool DoAABB2OverlapCapsule2D( const AABB2& box, const Capsule2D& capsule );
bool DoAABB2OverlapDisc( const AABB2& box, const Disc& disc );
bool DoAABB2OverlapLineSeg2D( const AABB2& box, const LineSeg2D& lineSeg );
bool DoAABB2OverlapOBB2( const AABB2& box1, const OBB2& box2 );

bool DoCapsule2DsOverlap( const Capsule2D& capsule1, const Capsule2D& capsule2 );
bool DoCapsule2DOverlapDisc( const Capsule2D& capsule, const Disc& disc );
bool DoCapsule2DOverlapLineSeg2D( const Capsule2D& capsule, const LineSeg2D& lineSeg );
bool DoCapsule2DOverlapOBB2( const Capsule2D& capsule, const OBB2& box );

bool DoDiscsOverlap( const Disc& disc1, const Disc& disc2 );
bool DoDiscOverlapLineSeg2D( const Disc& disc, const LineSeg2D& lineSeg );
bool DoDiscOverlapOBB2( const Disc& disc, const OBB2& box );

bool DoLineSeg2DsOVerlap( const LineSeg2D& lineSeg1, const LineSeg2D& lineSeg2 );
bool DoLineSeg2DOverlapOBB2( const LineSeg2D& lineSeg, const OBB2& box );

bool DoOBB2sOverlap( const OBB2& box1, OBB2 box2 );

bool DoSpheresOverlap( const Vec3& sphere1,
                       float radius1,
                       const Vec3& sphere2,
                       float radius2 );

//-------------------------------------------------------------------------------
// Vector Transforms
const Vec2 Transform2D( const Vec2& position,
                        float uniformScale,
                        float rotationDegrees,
                        const Vec2& translation );
const Vec2 Transform2D( const Vec2& position,
                        const Vec2& scale,
                        float rotationDegrees,
                        const Vec2& translation );
const Vec2 Transform2D( const Vec2& position,
                        const Vec2& iBasis,
                        const Vec2& jBasis,
                        const Vec2& translation );
const Vec3 Transform3DXY( const Vec3& position,
                          float uniformScale,
                          float rotationDegrees,
                          const Vec2& translation );
const Vec3 Transform3DXY( const Vec3& position,
                          const Vec2& scale,
                          float rotationDegrees,
                          const Vec2& translation );
const Vec3 Transform3DXY( const Vec3& position,
                          const Vec2& iBasis,
                          const Vec2& jBasis,
                          const Vec2& translation );

//-------------------------------------------------------------------------------
// Interpolations
float Clamp( float value, float min, float max );
float Wrap( float value, float min, float max );
float ClampZeroToOne( float value );
float Interpolate( float begin, float end, float fraction );
float InterpolateAndClamp( float begin, float end, float fraction );
float RangeMapFloat( float inputBegin,
                     float inputEnd,
                     float outputBegin,
                     float outputEnd,
                     float inputValue );
float RangeMapFloatZeroOne( float inputBegin,
                            float inputEnd,
                            float inputValue );
Vec2 RangeMapFloatVec2( float inputBegin, float inputEnd, const Vec2& outputBegin,
                         const Vec2& outputEnd, float inputValue );

//-----------------------------------------------------------------------------
// Rounding Functions
int ceili( float value );
int roundi( float value );
int floori( float value );

bool IsMostlyEqual( float a, float b, float epsilon = 1e-7 );
double Min( double a, double b );
double Max( double a, double b );
size_t Minu( size_t a, size_t b );
size_t Maxu( size_t a, size_t b );
float Minf( float a, float b );
float Maxf( float a, float b );
float Minf( float a, float b, float c );
float Maxf( float a, float b, float c );

int Signi( int a );
int Signi( float a );
int Signi( double a );
float Signf( int a );
float Signf( float a );
float Signf( double a );
double Sign( int a );
double Sign( float a );
double Sign( double a );

//-----------------------------------------------------------------------------
// Integer Functions
int absMod( int value, int modulus );
