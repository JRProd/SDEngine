#pragma once
#include "Vec2.hpp"

struct Vec4;
struct Vec2;

// Copy of Vec2 with name changed
// From Prof. Eiserloh
//-------------------------------------------------------------------------------
struct Vec3
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

public:
    // Construction/Destruction
    ~Vec3() {}												// destructor (do nothing)
    Vec3() {}												// default constructor (do nothing)
    Vec3( const Vec3& copyFrom );							// copy constructor (from another vec3)
    explicit Vec3( const Vec2& copyFrom );
    explicit Vec3( const Vec2& copyFrom, float initialZ );
    explicit Vec3( const Vec4& copyFrom );
    explicit Vec3( float allThree );
    explicit Vec3( float initialX, float initialY, float initialZ = 0.f );		// explicit constructor (from x, y)
    const Vec3 Copy() const;

    // Static Methods
    static const Vec3 MakeFromPolarDegreesXY( float angleDegrees, 
                                              float length = 1.f );

    static const Vec3 CreateFromString( const char* data );

    static float GetDistance( const Vec3& position1, const Vec3& position2 );
    static float GetDistanceSquared( const Vec3& position1, const Vec3& position2 );
    static float GetDistanceXY( const Vec3& position1, const Vec3& position2 );
    static float GetDistanceXYSquared( const Vec3& position1, const Vec3& position2 );
    static float Dot( const Vec3& vec1, const Vec3& vec2 );
    static Vec3 Lerp( const Vec3& vec1, const Vec3& vec2, float percentage );

    // Static Vec3
    static Vec3 ZERO;
    static Vec3 ONE;
    static Vec3 UNIT_EAST;
    static Vec3 UNIT_NORTH;
    static Vec3 UNIT_WEST;
    static Vec3 UNIT_SOUTH;
    static Vec3 I;
    static Vec3 J;
    static Vec3 K;

    // Accessors (const methods)
    float		GetLength() const;
    float		GetLengthXY() const;
    float		GetLengthSquared() const;
    float		GetLengthXYSquared() const;
    float		GetAngleAboutZDegrees() const;
    float		GetAngleAboutZRadians() const;
    const Vec3  GetCross( const Vec3& other ) const;
    float       GetProjectedLength( const Vec3& ontoVector ) const;
    const Vec3  GetProjectedOnto( const Vec3& ontoVector ) const;
    const Vec3	GetRotatedAboutZDegrees( float degrees ) const;
    const Vec3	GetRotatedAboutZRadians( float radians ) const;
    const Vec3	GetClamped( float maxLength ) const;
    const Vec3	GetNormalized() const;
    Vec3 GetLeft() const;

    // Swizzles (const methods)
    Vec2 XX() const { return Vec2( x, x ); }
    Vec2 XY() const { return Vec2( x, y ); }
    Vec2 XZ() const { return Vec2( x, z ); }
    Vec3 XXX() const { return Vec3( x, x, x ); }
    Vec3 XYZ() const { return Vec3( x, y, z ); }
    Vec3 XZY() const { return Vec3( x, z, y ); }

    Vec2 YX() const { return Vec2( y, x ); }
    Vec2 YY() const { return Vec2( y, y ); }
    Vec2 YZ() const { return Vec2( y, z ); }
    Vec3 YYY() const { return Vec3( y, y, y ); }
    Vec3 YXZ() const { return Vec3( y, x, z ); }
    Vec3 YZX() const { return Vec3( y, z, x ); }

    Vec2 ZX() const { return Vec2( z, x ); }
    Vec2 ZY() const { return Vec2( z, y ); }
    Vec2 ZZ() const { return Vec2( z, z ); }
    Vec3 ZZZ() const { return Vec3( z, z, z ); }
    Vec3 ZXY() const { return Vec3( z, x, y ); }
    Vec3 ZYX() const { return Vec3( z, y, x ); }


    // Mutators (non-const)

//     void		SetAngleDegrees( float degrees );
//     void		SetAngleRadians( float radians );
//     void		SetPolarDegrees( float degrees, float length );
//     void		SetPolarRadians( float radians, float length );
//     void		RotateMinus90Degrees();
//     void		RotateDegrees( float degrees );
//     void		RotateRadians( float radians );
    void		SetLength( float length );
    void        ReflectAcrossNormal( const Vec3& normalizedNormal );
    void Normalize();
    float		NormalizeAndGetPreviousLength();

    void ClampLength( float maxLength );

    // Operators (const)
    bool		operator==( const Vec3& compare ) const;		// vec3 == vec3
    bool		operator!=( const Vec3& compare ) const;		// vec3 != vec3
    const Vec3	operator+( const Vec3& vecToAdd ) const;		// vec3 + vec3
    const Vec3	operator-( const Vec3& vecToSubtract ) const;	// vec3 - vec3
    const Vec3	operator-() const;								// -vec3, i.e. "unary negation"
    const Vec3	operator*( float uniformScale ) const;			// vec3 * float
    const Vec3	operator*( const Vec3& vecToMultiply ) const;	// vec3 * vec3
    const Vec3	operator/( float inverseScale ) const;			// vec3 / float
    bool IsMostlyEqual( const Vec3& rhs, float epsilon = 1e-7f ) const;

    // Operators (self-mutating / non-const)
    void		operator+=( const Vec3& vecToAdd );				// vec3 += vec3
    void		operator-=( const Vec3& vecToSubtract );		// vec3 -= vec3
    void		operator*=( float uniformScale );			    // vec3 *= float
    void		operator*=( const Vec3& scale );			    // vec3 *= vec3
    void		operator/=( float uniformDivisor );		        // vec3 /= float
    void		operator=( const Vec3& copyFrom );				// vec3 = vec3

    // Standalone "friend" functions that are conceptually, but not actually, part of Vec2::
    friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3
};

std::string ConvertToString( const Vec3& value );
std::string GetTypeName( const Vec3& value );
Vec3 ConvertFromString( const char* string, const Vec3& defaultValue );
Vec3 ConvertFromString( const std::string& string, const Vec3& defaultValue );

