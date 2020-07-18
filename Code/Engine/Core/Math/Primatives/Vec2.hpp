#pragma once

#include <string>

struct Vec3;
struct IntVec2;

//-------------------------------------------------------------------------------
struct Vec2
{
    float x = 0.f;
    float y = 0.f;

public:
    // Construction/Destruction
    ~Vec2() {}												// destructor (do nothing)
    Vec2() {}												// default constructor (do nothing)
    Vec2( const Vec2& copyFrom );							// copy constructor (from another vec2)
    explicit Vec2( float both);
    explicit Vec2( float initialX, float initialY );		// explicit constructor (from x, y)
    explicit Vec2( const Vec3& copyFrom );
    explicit Vec2( const IntVec2& copyFrom );
    const Vec2 Copy() const;

    // Static methods
    static const Vec2 MakeFromPolarDegrees( float directionDegrees,
                                            float length = 1.f );
    static const Vec2 MakeFromPolarRadians( float directionRadians,
                                            float length = 1.f );

    static const Vec2 CreateFromString( const char* data );

    static float GetDistance( const Vec2& position1, const Vec2& position2 );
    static float GetDistanceSquared( const Vec2& position1, const Vec2& positon2 );
    static float GetAngleDegreesBetween( const Vec2& startingVal, const Vec2& endingVal );

    static float Dot( const Vec2& vec1, const Vec2& vec2 );
    static const Vec2 Lerp( const Vec2& percent, const Vec2& startingVal, const Vec2& endingVal );
    static const Vec2 ClampVector( const Vec2& vecToClamp,
                                   const Vec2& min = Vec2::ZERO,
                                   const Vec2& max = Vec2::ONE );

    // Static Vec2s
    static Vec2 ZERO;
    static Vec2 ONE;
    static Vec2 ZERO_ONE;
    static Vec2 ONE_ZERO;
    static Vec2 UNIT_EAST;
    static Vec2 UNIT_NORTH;
    static Vec2 UNIT_WEST;
    static Vec2 UNIT_SOUTH;

    // Static Aligns
    static Vec2 ALIGN_BOTTOM_LEFT;
    static Vec2 ALIGN_BOTTOM_CENTER;
    static Vec2 ALIGN_BOTTOM_RIGHT;
    static Vec2 ALIGN_CENTER_LEFT;
    static Vec2 ALIGN_CENTERED;
    static Vec2 ALIGN_CENTER_RIGHT;
    static Vec2 ALIGN_TOP_LEFT;
    static Vec2 ALIGN_TOP_CENTER;
    static Vec2 ALIGN_TOP_RIGHT;

    // Accessors (const methods)
    float		GetLength() const;
    float		GetLengthSquared() const;
    float		GetAngleDegrees() const;
    float		GetAngleRadians() const;
    Vec2	GetRotated90Degrees() const;
    Vec2	GetRotatedMinus90Degrees() const;
    Vec2	GetRotatedDegrees( float degrees ) const;
    Vec2	GetRotatedRadians( float radians ) const;
    float       GetProjectedLength( const Vec2& ontoVector ) const;
    Vec2  GetProjectionOnto( const Vec2& ontoVector ) const;
    Vec2	GetClamped( float maxLength = 1.f ) const;
    Vec2  GetReflectionAcrossNormal( const Vec2& normal ) const;
    Vec2	GetNormalized() const;
    Vec2 GetDirectionTo( const Vec2& location ) const;

    // Modifiers (non-const methods)
    void		SetAngleDegrees( float degrees );
    void		SetAngleRadians( float radians );
    void		SetPolarDegrees( float degrees, float length );
    void		SetPolarRadians( float radians, float length );
    void		Rotate90Degrees();
    void		RotateMinus90Degrees();
    void		RotateDegrees( float degrees );
    void		RotateRadians( float radians );
    void		SetLength( float length );
    void		ClampLength( float maxLength );
    void        ReflectAcrossNormal( const Vec2& normal );
    void		Normalize();
    float		NormalizeAndGetPreviousLength();

    // Operators (const)
    bool		operator==( const Vec2& compare ) const;		// vec2 == vec2
    bool		operator!=( const Vec2& compare ) const;		// vec2 != vec2
    const Vec2	operator+( const Vec2& vecToAdd ) const;		// vec2 + vec2
    const Vec2	operator-( const Vec2& vecToSubtract ) const;	// vec2 - vec2
    const Vec2	operator-() const;								// -vec2, i.e. "unary negation"
    const Vec2	operator*( float uniformScale ) const;			// vec2 * float
    const Vec2	operator*( const Vec2& vecToMultiply ) const;	// vec2 * vec2
    const Vec2	operator/( float inverseScale ) const;			// vec2 / float
    bool IsMostlyEqual( const Vec2& rhs, float epsilon = 1e-7f ) const;

    // Operators (self-mutating / non-const)
    void		operator+=( const Vec2& vecToAdd );				// vec2 += vec2
    void		operator-=( const Vec2& vecToSubtract );		// vec2 -= vec2
    void		operator*=( float uniformScale );			    // vec2 *= float
    void		operator*=( const Vec2& scale );			    // vec2 *= vec2
    void		operator/=( float uniformDivisor );		        // vec2 /= float
    void		operator=( const Vec2& copyFrom );				// vec2 = vec2

    // Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
    friend const Vec2 operator*( float uniformScale, const Vec2& vecToScale );	// float * vec2
};

//-----------------------------------------------------------------------------
// Math Unit Test Functions
float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector );
const Vec2 GetProjectionOnto2D( const Vec2& sourceVector, const Vec2& ontoVector );

std::string ConvertToString( const Vec2& value );
std::string GetTypeName( const Vec2& value );
Vec2 ConvertFromString( const char* string, const Vec2& defaultValue );
Vec2 ConvertFromString( const std::string& string, const Vec2& defaultValue );
