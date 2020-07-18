#include "Vec4.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"

Vec4::Vec4( const Vec4& copyFrom )
    : x( copyFrom.x )
    , y( copyFrom.y )
    , z( copyFrom.z )
    , w( copyFrom.w )
{
}

Vec4::Vec4( float allFour )
    : x( allFour )
    , y( allFour )
    , z( allFour )
    , w( allFour )
{
}

Vec4::Vec4( const Vec3& firstThree, float initialW )
    : Vec4(firstThree.x, firstThree.y, firstThree.z, initialW)
{
}

Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
    : x( initialX )
    , y( initialY )
    , z( initialZ )
    , w( initialW )
{
}

bool Vec4::operator==( const Vec4& rhs ) const
{
    return !this->operator!=( rhs );
}

bool Vec4::operator!=( const Vec4& rhs ) const
{
    return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
}

std::string ConvertToString( const Vec4& value )
{
    return Stringf("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
}

std::string GetTypeName( const Vec4& value )
{
    UNUSED( value );
    return std::string("Vec4");
}

/* Convert Vec4 from a formatted string
 - Format - (x,y,z,w) or x,y,z,w
 */
Vec4 ConvertFromString( const char* string, const Vec4& defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

/* Convert Vec4 from a formatted string
 - Format - (x,y,z,w) or x,y,z,w
 */
Vec4 ConvertFromString( const std::string& string, const Vec4& defaultValue )
{
    Vec4 result = defaultValue;
    const Strings split = SplitStringOnDelimiter( string, ',' );
    if ( split.size() == 4 )
    {
        if ( split.at( 0 ).front() == '(' )
        {
            result.x = ConvertFromString( split.at( 0 ).substr( 1 ), defaultValue.x );

            const size_t count = split.at( 2 ).size() - 1;
            result.w = ConvertFromString( split.at( 1 ).substr( 0, count ), defaultValue.w );
        }
        else
        {
            result.x = ConvertFromString( split.at( 0 ), defaultValue.x );
            result.w = ConvertFromString( split.at( 2 ), defaultValue.w );
        }

        // Y and Z does not have any special case
        result.y = ConvertFromString( split.at( 1 ), defaultValue.y );
        result.z = ConvertFromString( split.at( 1 ), defaultValue.z );
    }
    return result;
}

std::string CovertToString( const Vec4& value )
{
    return Stringf( "(%f,%f,%f,%f)", value.x, value.y, value.z, value.w );
}

STATIC Vec4 Vec4::ZERO = Vec4( 0, 0, 0, 0 );
STATIC Vec4 Vec4::ONE = Vec4( 1,1,1,1 );
STATIC Vec4 Vec4::I_BASIS = Vec4( 1,0,0,0 );
STATIC Vec4 Vec4::J_BASIS = Vec4( 0,1,0,0 );
STATIC Vec4 Vec4::K_BASIS = Vec4( 0,0,1,0 );
STATIC Vec4 Vec4::T_BASIS = Vec4( 0,0,0,1 );