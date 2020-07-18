#include "IntVec2.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"

IntVec2::IntVec2( const IntVec2& copyFrom )
    : x( copyFrom.x )
    , y( copyFrom.y )
{
}

IntVec2::IntVec2( int initialX, int initialY )
    : x( initialX )
    , y( initialY )
{
}

IntVec2::IntVec2( const Vec2& copyFrom )
    : x( static_cast<int>(copyFrom.x) )
    , y( static_cast<int>(copyFrom.y) )
{
}

STATIC IntVec2 IntVec2::CreateFromString( const char* data )
{
    Strings splitIntVec2 = SplitStringOnDelimiter( data, ',' );

    IntVec2 value;
    if ( splitIntVec2.size() == 2 )
    {
        value.x = stoi( splitIntVec2.at( 0 ) );
        value.y = stoi( splitIntVec2.at( 1 ) );
    }
    else
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "IntVec2::CreateFromString - Recieved invalid sequence \"%s\"", data ) );
    #endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

STATIC int IntVec2::GetTaxicabDistance( const IntVec2& posA, const IntVec2& posB )
{
    return abs(posB.x - posA.x) + abs(posB.y - posA.y);
}

float IntVec2::GetLength() const
{
    return sqrtf( ( float) GetLengthSquared() );
}

int IntVec2::GetLengthSquared() const
{
    return (x * x) + (y * y);
}

float IntVec2::GetAngleDegrees() const
{
    return atan2fDegrees( ( float) y, ( float) x );
}

float IntVec2::GetAngleRadians() const
{
    return atan2f((float) y, (float) x);
}

int IntVec2::GetTaxiCabLength() const
{
    return abs( x ) + abs( y );
}

const IntVec2 IntVec2::GetRotated90Degrees() const
{
    return IntVec2( -y, x );
}

const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
    return IntVec2( y, -x );
}

void IntVec2::Rotate90Degrees()
{
    int temp = x;
    x = -y;
    y = temp;
}

void IntVec2::RotateMinus90Degrees()
{
    int temp = -x;
    x = y;
    y = temp;
}

bool IntVec2::operator==( const IntVec2& compare ) const
{
    return x == compare.x && y == compare.y;
}

bool IntVec2::operator!=( const IntVec2& compare ) const
{
    return x != compare.x || y != compare.y;
}

const IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
    return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}

const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
    return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const IntVec2 IntVec2::operator-() const
{
    return IntVec2( -x, -y );
}

const IntVec2 IntVec2::operator*( int uniformScale ) const
{
    return IntVec2( x * uniformScale, y * uniformScale );
}

const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
    return IntVec2( x * vecToMultiply.x, y * vecToMultiply.y );
}

const IntVec2 IntVec2::operator/( int inverseScale ) const
{
    return IntVec2( x / inverseScale, y / inverseScale );
}

void IntVec2::operator+=( const IntVec2& vecToAdd )
{
    x += vecToAdd.x;
    y += vecToAdd.y;
}

void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
    x -= vecToSubtract.x;
    y -= vecToSubtract.y;
}

void IntVec2::operator*=( int uniformScale )
{
    x *= uniformScale;
    y *= uniformScale;
}

void IntVec2::operator/=( int uniformDivisor )
{
    x /= uniformDivisor;
    y /= uniformDivisor;
}

void IntVec2::operator=( const IntVec2& copyFrom )
{
    x = copyFrom.x;
    y = copyFrom.y;
}

const std::string IntVec2::ToString() const
{
    return Stringf("%d,%d", x, y);
}

const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
    return IntVec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}

std::string ConvertToString( const IntVec2& value )
{
    return Stringf("(%d,%d)", value.x, value.y);
}

std::string GetTypeName( const IntVec2& value )
{
    UNUSED( value );

    return std::string("IntVec2");
}

/* Convert Vec2 from a formatted string
 - Format - (x,y) or x,y
 */
IntVec2 ConvertFromString( const char* string, const IntVec2& defaultValue )
{
    return ConvertFromString( std::string(string), defaultValue );
}

/* Convert Vec2 from a formatted string
 - Format - (x,y) or x,y
 */
IntVec2 ConvertFromString( const std::string& string, const IntVec2& defaultValue )
{
    IntVec2 result = defaultValue;
    const Strings split = SplitStringOnDelimiter( string, ',' );
    if( split.size() == 2 )
    {
        if( split.at( 0 ).front() == '(' )
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

STATIC IntVec2 IntVec2::ZERO = IntVec2( 0, 0 );
STATIC IntVec2 IntVec2::ONE = IntVec2( 1, 1 );
STATIC IntVec2 IntVec2::UNIT_EAST = IntVec2( 0, 1 );
STATIC IntVec2 IntVec2::UNIT_NORTH = IntVec2( 1, 0 );
STATIC IntVec2 IntVec2::UNIT_WEST = IntVec2( 0, -1 );
STATIC IntVec2 IntVec2::UNIT_SOUTH = IntVec2( -1, 0 );