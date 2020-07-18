#include "Engine/Core/Math/Primatives/Vec2.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"

//-----------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
    : x( copy.x )
      , y( copy.y )
{
}

Vec2::Vec2( float both )
    : x( both )
      , y( both )
{
}


//-----------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
    : x( initialX )
      , y( initialY )
{
}

Vec2::Vec2( const Vec3& copyFrom )
    : x( copyFrom.x )
      , y( copyFrom.y )
{
}

Vec2::Vec2( const IntVec2& copyFrom )
    : x( static_cast<float>(copyFrom.x) )
      , y( static_cast<float>(copyFrom.y) )
{
}

//-----------------------------------------------------------------------------
const Vec2 Vec2::Copy() const
{
    return Vec2( x, y );
}

//-----------------------------------------------------------------------------
STATIC const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length )
{
    return Vec2( length * cosfDegrees( directionDegrees ),
                 length * sinfDegrees( directionDegrees ) );
}

//-----------------------------------------------------------------------------
STATIC const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length )
{
    return Vec2( length * cosf( directionRadians ),
                 length * sinf( directionRadians ) );
}

STATIC const Vec2 Vec2::CreateFromString( const char* data )
{
    Strings splitVec2 = SplitStringOnDelimiter( data, ',' );

    Vec2 value;
    if ( splitVec2.size() == 2 )
    {
        value.x = stof( splitVec2.at( 0 ) );
        value.y = stof( splitVec2.at( 1 ) );
    }
    else
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR,
                        Stringf( "Vec2::CreateFromString - Recieved invalid sequence \"%s\"",
                                 data ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

//-----------------------------------------------------------------------------
STATIC float Vec2::GetDistance( const Vec2& position1, const Vec2& position2 )
{
    return sqrtf( GetDistanceSquared( position1, position2 ) );
}

//-----------------------------------------------------------------------------
STATIC float Vec2::GetDistanceSquared( const Vec2& position1, const Vec2& position2 )
{
    return (position2.x - position1.x) * (position2.x - position1.x) +
        (position2.y - position1.y) * (position2.y - position1.y);
}

STATIC float Vec2::GetAngleDegreesBetween( const Vec2& startingVal, const Vec2& endingVal )
{
    float arc = Dot( startingVal, endingVal ) / (startingVal.GetLength() * endingVal.GetLength());
    return ConvertRadiansToDegrees( acosf( arc ) );
}

//-----------------------------------------------------------------------------
STATIC float Vec2::Dot( const Vec2& vec1, const Vec2& vec2 )
{
    return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

STATIC const Vec2 Vec2::Lerp( const Vec2& percent, const Vec2& startingVal, const Vec2& endingVal )
{
    return Vec2( Interpolate( startingVal.x, endingVal.x, percent.x ),
                 Interpolate( startingVal.y, endingVal.y, percent.y ) );
}

STATIC const Vec2 Vec2::ClampVector( const Vec2& vecToClamp, const Vec2& min, const Vec2& max )
{
    return Vec2( Clamp( vecToClamp.x, min.x, max.x ),
                 Clamp( vecToClamp.y, min.y, max.y ) );
}

//-----------------------------------------------------------------------------
float Vec2::GetLength() const
{
    return sqrtf( GetLengthSquared() );
}

//-----------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
    return (x * x) + (y * y);
}

//-----------------------------------------------------------------------------
float Vec2::GetAngleDegrees() const
{
    if ( x == 0 && y == 0 ) { return 0.f; }
    return atan2fDegrees( y, x );
}

//-----------------------------------------------------------------------------
float Vec2::GetAngleRadians() const
{
    return atan2f( y, x );
}

//-----------------------------------------------------------------------------
Vec2 Vec2::GetRotated90Degrees() const
{
    return Vec2( y * -1, x );
}

//-----------------------------------------------------------------------------
Vec2 Vec2::GetRotatedMinus90Degrees() const
{
    return Vec2( y, x * -1 );
}


//-----------------------------------------------------------------------------
Vec2 Vec2::GetRotatedDegrees( float degrees ) const
{
    float distance = GetLength();
    float thetaDegrees = atan2fDegrees( y, x );
    thetaDegrees += degrees;

    return Vec2( distance * cosfDegrees( thetaDegrees ),
                 distance * sinfDegrees( thetaDegrees ) );
}

//-----------------------------------------------------------------------------
Vec2 Vec2::GetRotatedRadians( float radians ) const
{
    float distance = GetLength();
    float thetaRadians = atan2f( y, x );
    thetaRadians += radians;

    return Vec2( distance * cosf( thetaRadians ),
                 distance * sinf( thetaRadians ) );
}

//-----------------------------------------------------------------------------
float Vec2::GetProjectedLength( const Vec2& ontoVector ) const
{
    return Dot( *this, ontoVector.GetNormalized() );
}

//-----------------------------------------------------------------------------
Vec2 Vec2::GetProjectionOnto( const Vec2& ontoVector ) const
{
    Vec2 dotResult = Dot( *this, ontoVector ) * ontoVector;
    return dotResult / ontoVector.GetLengthSquared();
}

//-----------------------------------------------------------------------------
Vec2 Vec2::GetClamped( float maxLength ) const
{
    if ( GetLengthSquared() > maxLength * maxLength )
    {
        return GetNormalized() * maxLength;
    }
    return this->Copy();
}

Vec2 Vec2::GetReflectionAcrossNormal( const Vec2& normal ) const
{
    Vec2 normalizedNormal = normal.GetNormalized();
    return operator-( 2 * (Dot( *this, normalizedNormal ) * normalizedNormal) );
}

//-----------------------------------------------------------------------------
Vec2 Vec2::GetNormalized() const
{
    if ( x == 0 && y == 0 ) { return Vec2( 0, 0 ); }
    float normalizationFactor = 1.0f / GetLength();
    return Vec2( x * normalizationFactor, y * normalizationFactor );
}

Vec2 Vec2::GetDirectionTo( const Vec2& location ) const
{
    const Vec2 displacement = location - *this;
    return displacement.GetNormalized();
}

//-----------------------------------------------------------------------------
void Vec2::SetAngleDegrees( float degrees )
{
    float distance = GetLength();

    x = distance * cosfDegrees( degrees );
    y = distance * sinfDegrees( degrees );
}

//-----------------------------------------------------------------------------
void Vec2::SetAngleRadians( float radians )
{
    float distance = GetLength();

    x = distance * cosf( radians );
    y = distance * sinf( radians );
}

//-----------------------------------------------------------------------------
void Vec2::SetPolarDegrees( float degrees, float length )
{
    x = length * cosfDegrees( degrees );
    y = length * sinfDegrees( degrees );
}

//-----------------------------------------------------------------------------
void Vec2::SetPolarRadians( float radians, float length )
{
    x = length * cosf( radians );
    y = length * sinf( radians );
}

//-----------------------------------------------------------------------------
void Vec2::Rotate90Degrees()
{
    float rotateTemp = y * -1;
    y = x;
    x = rotateTemp;
}

//-----------------------------------------------------------------------------
void Vec2::RotateMinus90Degrees()
{
    float rotateTemp = x * -1;
    x = y;
    y = rotateTemp;
}

//-----------------------------------------------------------------------------
void Vec2::RotateDegrees( float degrees )
{
    float length = GetLength();
    float thetaDegrees = atan2fDegrees( y, x );
    thetaDegrees += degrees;

    x = length * cosfDegrees( thetaDegrees );
    y = length * sinfDegrees( thetaDegrees );
}

//-----------------------------------------------------------------------------
void Vec2::RotateRadians( float radians )
{
    float length = GetLength();
    float thetaRadians = atan2f( y, x );
    thetaRadians += radians;

    x = length * cosf( thetaRadians );
    y = length * sinf( thetaRadians );
}

//-----------------------------------------------------------------------------
void Vec2::SetLength( float length )
{
    Normalize();
    x *= length;
    y *= length;
}

//-----------------------------------------------------------------------------
void Vec2::ClampLength( float maxLength )
{
    if ( GetLengthSquared() > maxLength * maxLength )
    {
        Normalize();
        x *= maxLength;
        y *= maxLength;
    }
}

void Vec2::ReflectAcrossNormal( const Vec2& normal )
{
    Vec2 normalizedNormal = normal.GetNormalized();
    operator-=( 2 * (Dot( *this, normalizedNormal ) * normalizedNormal) );
}

//-----------------------------------------------------------------------------
void Vec2::Normalize()
{
    if ( x == 0 && y == 0 ) { return; }
    float normaliationFactor = 1.f / GetLength();

    x *= normaliationFactor;
    y *= normaliationFactor;
}

//---------------------------------------------------------------------------
float Vec2::NormalizeAndGetPreviousLength()
{
    float length = GetLength();
    Normalize();

    return length;
}

//-----------------------------------------------------------------------------
const Vec2 Vec2::operator +( const Vec2& vecToAdd ) const
{
    return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
    return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
    return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
    return Vec2( x * uniformScale, y * uniformScale );
}


//------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
    return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}


//-----------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
    return Vec2( x / inverseScale, y / inverseScale );
}

//-----------------------------------------------------------------------------
bool Vec2::IsMostlyEqual( const Vec2& rhs, float epsilon ) const
{
    return ::IsMostlyEqual( x, rhs.x, epsilon ) && ::IsMostlyEqual( y, rhs.y, epsilon );
}

//-----------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
}


//-----------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------
void Vec2::operator*=( float uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
}

void Vec2::operator*=( const Vec2& scale )
{
    x *= scale.x;
    y *= scale.y;
}


//-----------------------------------------------------------------------------
void Vec2::operator/=( float uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
}


//-----------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
}

//-----------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
    return Vec2( uniformScale * vecToScale.x, uniformScale * vecToScale.y );
}

//-----------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
    return x == compare.x && y == compare.y;
}

//-----------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
    return x != compare.x || y != compare.y;
}

STATIC Vec2 Vec2::ZERO = Vec2( 0.f, 0.f );
STATIC Vec2 Vec2::ONE = Vec2( 1.f, 1.f );
STATIC Vec2 Vec2::ZERO_ONE = Vec2( 0.f, 1.f );
STATIC Vec2 Vec2::ONE_ZERO = Vec2( 1.f, 0.f );
STATIC Vec2 Vec2::UNIT_EAST = Vec2( 1.f, 0.f );
STATIC Vec2 Vec2::UNIT_NORTH = Vec2( 0.f, 1.f );
STATIC Vec2 Vec2::UNIT_WEST = Vec2( -1.f, 0.f );
STATIC Vec2 Vec2::UNIT_SOUTH = Vec2( 0.f, -1.f );

STATIC Vec2 Vec2::ALIGN_BOTTOM_LEFT = Vec2( 0.f, 0.f );
STATIC Vec2 Vec2::ALIGN_BOTTOM_CENTER = Vec2( .5f, 0.f );
STATIC Vec2 Vec2::ALIGN_BOTTOM_RIGHT = Vec2( 1.f, 0.f );
STATIC Vec2 Vec2::ALIGN_CENTER_LEFT = Vec2( 0.f, .5f );
STATIC Vec2 Vec2::ALIGN_CENTERED = Vec2( .5f, .5f );
STATIC Vec2 Vec2::ALIGN_CENTER_RIGHT = Vec2( 1.f, .5f );
STATIC Vec2 Vec2::ALIGN_TOP_LEFT = Vec2( 0.f, 1.f );
STATIC Vec2 Vec2::ALIGN_TOP_CENTER = Vec2( .5f, 1.f );
STATIC Vec2 Vec2::ALIGN_TOP_RIGHT = Vec2( 1.f, 1.f );

//-----------------------------------------------------------------------------
float GetProjectedLength2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
    return sourceVector.GetProjectedLength( ontoVector );
}

//-----------------------------------------------------------------------------
const Vec2 GetProjectionOnto2D( const Vec2& sourceVector, const Vec2& ontoVector )
{
    return sourceVector.GetProjectionOnto( ontoVector );
}

std::string ConvertToString( const Vec2& value )
{
    return Stringf("(%f,%f)", value.x, value.y );
}

std::string GetTypeName( const Vec2& value )
{
    UNUSED( value );
   return std::string("Vec2");
}

/* Convert Vec2 from a formatted string
 - Format - (x,y) or x,y
 */
Vec2 ConvertFromString( const char* string, const Vec2& defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

/* Convert Vec2 from a formatted string
 - Format - (x,y) or x,y
 */
Vec2 ConvertFromString( const std::string& string, const Vec2& defaultValue )
{
    Vec2 result = defaultValue;
    const Strings split = SplitStringOnDelimiter( string, ',' );
    if ( split.size() == 2 )
    {
        if ( split.at( 0 ).front() == '(' )
        {
            result.x = ConvertFromString( split.at( 0 ).substr( 1 ), defaultValue.x );

            const size_t count = split.at( 1 ).size() - 1;
            result.y = ConvertFromString( split.at( 1 ).substr( 0, count ), defaultValue.y );
        }
        else
        {
            result.x = ConvertFromString( split.at( 0 ), defaultValue.x );
            result.y = ConvertFromString( split.at( 1 ), defaultValue.y );
        }
    }
    return result;
}
