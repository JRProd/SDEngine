#include "Vec3.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"

#include "Engine/Core/Math/Primatives/Vec4.hpp"

//-----------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
    : x( copy.x )
      , y( copy.y )
      , z( copy.z )
{
}

//-----------------------------------------------------------------------------s
Vec3::Vec3( const Vec2& copyFrom )
    : x( copyFrom.x )
      , y( copyFrom.y )
      , z( 0.f )
{
}

Vec3::Vec3( const Vec2& copyFrom, float initialZ )
    : x( copyFrom.x )
      , y( copyFrom.y )
      , z( initialZ )
{
}

Vec3::Vec3( const Vec4& copyFrom )
    : x( copyFrom.x )
      , y( copyFrom.y )
      , z( copyFrom.z )
{
}

Vec3::Vec3( float allThree )
    : x( allThree )
      , y( allThree )
      , z( allThree )
{
}

//-----------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
    : x( initialX )
      , y( initialY )
      , z( initialZ )
{
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::Copy() const
{
    return Vec3( x, y, z );
}

//-----------------------------------------------------------------------------
STATIC const Vec3 Vec3::MakeFromPolarDegreesXY( float angleDegrees, float length )
{
    return Vec3( length * cosfDegrees( angleDegrees ),
                 length * sinfDegrees( angleDegrees ),
                 0.f );
}

STATIC const Vec3 Vec3::CreateFromString( const char* data )
{
    Strings splitVec3 = SplitStringOnDelimiter( data, ',' );

    Vec3 value;
    if ( splitVec3.size() == 3 )
    {
        value.x = stof( splitVec3.at( 0 ) );
        value.y = stof( splitVec3.at( 1 ) );
        value.z = stof( splitVec3.at( 2 ) );
    }
    else
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_FATAL,
                        Stringf( "Vec3::CreateFromString - Recieved invalid sequence \"%s\"",
                                 data ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

//-----------------------------------------------------------------------------
STATIC float Vec3::GetDistance( const Vec3& position1, const Vec3& position2 )
{
    return sqrtf( GetDistanceSquared( position1, position2 ) );
}

//-----------------------------------------------------------------------------
STATIC float Vec3::GetDistanceSquared( const Vec3& position1, const Vec3& position2 )
{
    return (position2.x - position1.x) * (position2.x - position1.x) +
        (position2.y - position1.y) * (position2.y - position1.y) +
        (position2.z - position1.z) * (position2.z - position1.z);
}

//-----------------------------------------------------------------------------
STATIC float Vec3::GetDistanceXY( const Vec3& position1, const Vec3& position2 )
{
    return sqrtf( GetDistanceXYSquared( position1, position2 ) );
}

//-----------------------------------------------------------------------------
STATIC float Vec3::GetDistanceXYSquared( const Vec3& position1, const Vec3& position2 )
{
    return (position2.x - position1.x) * (position2.x - position1.x) +
        (position2.y - position1.y) * (position2.y - position1.y);
}

//-----------------------------------------------------------------------------
STATIC float Vec3::Dot( const Vec3& vec1, const Vec3& vec2 )
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

Vec3 Vec3::Lerp( const Vec3& vec1, const Vec3& vec2, float percentage )
{
    return Vec3( Interpolate( vec1.x, vec2.x, percentage ),
                 Interpolate( vec1.y, vec2.y, percentage ),
                 Interpolate( vec1.z, vec2.z, percentage ) );
}

//-----------------------------------------------------------------------------
float Vec3::GetLength() const
{
    return sqrtf( GetLengthSquared() );
}

//-----------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
    return sqrtf( GetLengthXYSquared() );
}

//-----------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
    return (x * x) + (y * y) + (z * z);
}

//-----------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
    return (x * x) + (y * y);
}

//-----------------------------------------------------------------------------
float Vec3::GetAngleAboutZDegrees() const
{
    return atan2fDegrees( y, x );
}

//-----------------------------------------------------------------------------
float Vec3::GetAngleAboutZRadians() const
{
    return atan2f( y, x );
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::GetCross( const Vec3& other ) const
{
    return Vec3( (y * other.z) - (z * other.y),
                 (z * other.x) - (x * other.z),
                 (x * other.y) - (y * other.x) );
}

//-----------------------------------------------------------------------------
float Vec3::GetProjectedLength( const Vec3& ontoVector ) const
{
    return Dot( *this, ontoVector );
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::GetProjectedOnto( const Vec3& ontoVector ) const
{
    float dot = Dot( *this, ontoVector );
    return ontoVector * dot;
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZDegrees( float degrees ) const
{
    float length = GetLengthXY();
    float thetaDegrees = GetAngleAboutZDegrees();
    thetaDegrees += degrees;

    return Vec3( length * cosfDegrees( thetaDegrees ),
                 length * sinfDegrees( thetaDegrees ),
                 z );
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZRadians( float radians ) const
{
    float length = GetLengthXY();
    float thetaRadians = GetAngleAboutZRadians();
    thetaRadians += radians;

    return Vec3( length * cosf( thetaRadians ),
                 length * sinf( thetaRadians ), z );
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::GetClamped( float maxLength ) const
{
    if ( GetLengthSquared() > maxLength * maxLength )
    {
        return GetNormalized() * maxLength;
    }
    return this->Copy();
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::GetNormalized() const
{
    if ( x == 0.f && y == 0.f && z == 0.f ) { return Vec3( 0, 0, 0 ); }
    float normalizationFactor = 1.0f / GetLength();
    return Vec3( x * normalizationFactor,
                 y * normalizationFactor,
                 z * normalizationFactor );
}

Vec3 Vec3::GetLeft() const
{
    Vec2 xyPortion = Vec2( x, y );
    if ( ::IsMostlyEqual( x, 0.f ) && ::IsMostlyEqual( y, 0.f ) )
    {
        xyPortion.x = z > 0 ? -1.f : 1.f;
        xyPortion.y = 0.f;
    }
    else
    {
        xyPortion.Rotate90Degrees();
    }

    const Vec3 left = Vec3( xyPortion, 0.f );
    return left.GetNormalized();
}

//-----------------------------------------------------------------------------
void Vec3::SetLength( float newLength )
{
    Normalize();
    x *= newLength;
    y *= newLength;
    z *= newLength;
}

//-----------------------------------------------------------------------------
void Vec3::ReflectAcrossNormal( const Vec3& normalizedNormal )
{
    operator-=( 2 * (Dot( *this, normalizedNormal ) * normalizedNormal) );
}

//-----------------------------------------------------------------------------
void Vec3::Normalize()
{
    if ( x == 0.f && y == 0.f && z == 0.f ) { return; }
    float normalizationFactor = 1.0f / GetLength();

    x *= normalizationFactor;
    y *= normalizationFactor;
    z *= normalizationFactor;
}

float Vec3::NormalizeAndGetPreviousLength()
{
    const float length = GetLength();
    Normalize();

    return length;
}

//-----------------------------------------------------------------------------
void Vec3::ClampLength( float maxLength )
{
    if ( GetLengthSquared() > maxLength * maxLength )
    {
        Normalize();
        operator*=( maxLength );
    }
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::operator+( const Vec3& vecToAdd ) const
{
    return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}


//-----------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
    return Vec3( x - vecToSubtract.x,
                 y - vecToSubtract.y,
                 z - vecToSubtract.z );
}


//------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
    return Vec3( -x, -y, -z );
}


//-----------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
    return Vec3( x * uniformScale, y * uniformScale, z * uniformScale );
}


//------------------------------------------------------------------------------
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
    return Vec3( x * vecToMultiply.x,
                 y * vecToMultiply.y,
                 z * vecToMultiply.z );
}

//-----------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
    return Vec3( x / inverseScale, y / inverseScale, z / inverseScale );
}

bool Vec3::IsMostlyEqual( const Vec3& rhs, const float epsilon ) const
{
    return ::IsMostlyEqual( x, rhs.x, epsilon ) && ::IsMostlyEqual( y, rhs.y, epsilon ) && 
        ::IsMostlyEqual( z, rhs.z, epsilon );
}


//-----------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
    z += vecToAdd.z;
}


//-----------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
    z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
    z *= uniformScale;
}

void Vec3::operator*=( const Vec3& scale )
{
    x *= scale.x;
    y *= scale.y;
    z *= scale.z;
}


//-----------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
    z /= uniformDivisor;
}


//-----------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
    z = copyFrom.z;
}

//-----------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
    return Vec3( uniformScale * vecToScale.x,
                 uniformScale * vecToScale.y,
                 uniformScale * vecToScale.z );
}


//-----------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
    return x == compare.x && y == compare.y && z == compare.z;
}


//-----------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
    return x != compare.x || y != compare.y || z != compare.z;
}

STATIC Vec3 Vec3::ZERO = Vec3( 0.f, 0.f, 0.f );
STATIC Vec3 Vec3::ONE = Vec3( 1.f, 1.f, 1.f );
STATIC Vec3 Vec3::UNIT_EAST = Vec3( 1.f, 0.f, 0.f );
STATIC Vec3 Vec3::UNIT_NORTH = Vec3( 0.f, 1.f, 0.f );
STATIC Vec3 Vec3::UNIT_WEST = Vec3( -1.f, 0.f, 0.f );
STATIC Vec3 Vec3::UNIT_SOUTH = Vec3( 0.f, -1.f, 0.f );
STATIC Vec3 Vec3::I = Vec3( 1.f, 0.f, 0.f );
STATIC Vec3 Vec3::J = Vec3( 0.f, 1.f, 0.f );
STATIC Vec3 Vec3::K = Vec3( 0.f, 0.f, 1.f );

std::string ConvertToString( const Vec3& value )
{
    return Stringf( "(%f,%f,%f)", value.x, value.y, value.z );
}

std::string GetTypeName( const Vec3& value )
{
    UNUSED( value );
    return std::string("Vec3");
}


/* Convert Vec3 from a formatted string
 - Format - (x,y,z) or x,y,z
 */
Vec3 ConvertFromString( const char* string, const Vec3& defaultValue )
{
    return ConvertFromString( std::string(string), defaultValue );
}

/* Convert Vec3 from a formatted string
 - Format - (x,y,z) or x,y,z
 */
Vec3 ConvertFromString( const std::string& string, const Vec3& defaultValue )
{
    Vec3 result = defaultValue;
    const Strings split = SplitStringOnDelimiter( string, ',' );
    if ( split.size() == 3 )
    {
        if ( split.at( 0 ).front() == '(' )
        {
            result.x = ConvertFromString( split.at( 0 ).substr( 1 ), defaultValue.x );

            const size_t count = split.at( 2 ).size() - 1;
            result.z = ConvertFromString( split.at( 1 ).substr( 0, count ), defaultValue.z );
        }
        else
        {
            result.x = ConvertFromString( split.at( 0 ), defaultValue.x );
            result.z = ConvertFromString( split.at( 2 ), defaultValue.z );
        }

        // Y does not have any special case
        result.y = ConvertFromString( split.at( 1 ), defaultValue.y );
    }
    return result;
}