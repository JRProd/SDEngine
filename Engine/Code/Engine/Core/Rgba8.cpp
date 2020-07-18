#include "Rgba8.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"

unsigned char FromFloat( const float value )
{
    return static_cast<unsigned char>(value * 255.f);
}

Vec3 ConvertRgbToHSL( const Rgba8& rgba8 )
{
    const Vec3 hsl( static_cast<float>(rgba8.r) / 255.f, static_cast<float>(rgba8.g) / 255.f,
                    static_cast<float>(rgba8.b) / 255.f
                  );

    const float cMax = Maxf( hsl.x, hsl.y, hsl.z );
    const float cMin = Minf( hsl.x, hsl.y, hsl.z );
    const float delta = cMax - cMin;

    const float lightness = (cMax + cMin) * .5f;
    if( IsMostlyEqual( delta, 0.f ) ) { return Vec3( 0.f, 0.f, lightness ); }

    float hue = 0.f;
    if( IsMostlyEqual( cMax, hsl.x ) )
    {
        hue = 60.f * (static_cast<int>((hsl.y - hsl.z) / delta) % 6);
    }
    if( IsMostlyEqual( cMax, hsl.y ) )
    {
        hue = 60.f * ((hsl.z - hsl.x) / delta + 2.f);
    }
    if( IsMostlyEqual( cMax, hsl.z ) )
    {
        hue = 60.f * ((hsl.x - hsl.y) / delta + 4.f);
    }

    const float saturation = delta / (1 - abs( 2.f * lightness - 1 ));

    return Vec3( hue, saturation, lightness );
}

Rgba8 ConvertHSLToRgb( const Vec3& hsl )
{
    const float c = (1 - abs( 2.f * hsl.z - 1 )) * hsl.y;
    float hfract = hsl.x / 60.f;
    hfract = hfract - static_cast<int>(hfract / 2) * 2;
    const float x = c * static_cast<float>(1 - abs( hfract - 1 ));

    Vec3 rgbP;
    if( hsl.x < 60 )
    {
        rgbP = Vec3( c, x, 0 );
    }
    else if( hsl.x < 120 )
    {
        rgbP = Vec3( x, c, 0 );
    }
    else if( hsl.x < 180 )
    {
        rgbP = Vec3( 0, c, x );
    }
    else if( hsl.x < 240 )
    {
        rgbP = Vec3( 0, x, c );
    }
    else if( hsl.x < 300 )
    {
        rgbP = Vec3( x, 0, c );
    }
    else
    {
        rgbP = Vec3( c, 0, x );
    }

    const float m = hsl.z - c * .5f;
    Rgba8 rgba8( static_cast<unsigned char>((rgbP.x + m) * 255),
                 static_cast<unsigned char>((rgbP.y + m) * 255),
                 static_cast<unsigned char>((rgbP.z + m) * 255)
               );

    return rgba8;
}

//-------------------------------------------------------------------------------
Rgba8::Rgba8()
    : r( 255 )
  , g( 255 )
  , b( 255 )
  , a( 255 )
{
}

//-------------------------------------------------------------------------------
Rgba8::Rgba8( unsigned char initialR,
              unsigned char initialG,
              unsigned char initialB,
              unsigned char initialA )
    : r( initialR )
  , g( initialG )
  , b( initialB )
  , a( initialA )
{
}

Rgba8::Rgba8( const Rgba8& copy )
    : r( copy.r )
  , g( copy.g )
  , b( copy.b )
  , a( copy.a )
{
}

static Rgba8 CreateFromHex( const char* data )
{
    Rgba8 color;

    size_t length = 0;
    while( data[ length ] != '\0' )
    {
        length++;
    }

    // Hex are doubled
    if( length == 3 || length == 4 )
    {
        color.r = StringToHex( data[ 0 ] );
        color.g = StringToHex( data[ 1 ] );
        color.b = StringToHex( data[ 2 ] );
        if( length == 4 )
        {
            color.a = StringToHex( data[ 3 ] );
        }
    }
    else if( length == 6 || length == 8 )
    {
        color.r = StringToHex( data[ 0 ], data[ 1 ] );
        color.g = StringToHex( data[ 2 ], data[ 3 ] );
        color.b = StringToHex( data[ 4 ], data[ 5 ] );
        if( length == 8 )
        {
            color.a = StringToHex( data[ 6 ], data[ 7 ] );
        }
    }
    else
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR,
                        Stringf( "Rgba8::CreateFromString - Received invalid sequence \"%s\"",
                                 data
                               )
                      );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return color;
}

static Rgba8 CreateFromCommaSeparatedString( const char* data )
{
    Strings splitRgba = SplitStringOnDelimiter( data, ',' );

    Rgba8 value;
    if( splitRgba.size() >= 3 && splitRgba.size() <= 4 )
    {
        value.r = static_cast<unsigned char>(std::stoi( splitRgba.at( 0 ) ));
        value.g = static_cast<unsigned char>(std::stoi( splitRgba.at( 1 ) ));
        value.b = static_cast<unsigned char>(std::stoi( splitRgba.at( 2 ) ));

        if( splitRgba.size() == 4 )
        {
            value.a = static_cast<unsigned char>(std::stoi( splitRgba.at( 3 ) ));
        }
    }
    else
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR,
                        Stringf( "Rgba8::CreateFromString - Received invalid sequence \"%s\"",
                                 data
                               )
                      );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

STATIC Rgba8 Rgba8::CreateFromString( const char* data )
{
    if( data[ 0 ] == '0' && data[ 1 ] == 'x' )
    {
        // Remove the 0x at the beginning
        return CreateFromHex( &data[ 2 ] );
    }
    else
    {
        return CreateFromCommaSeparatedString( data );
    }
}

STATIC Rgba8 Rgba8::CreateFromVec3( const Vec3& rgb )
{
    return Rgba8( FromFloat( rgb.x ), FromFloat( rgb.y ), FromFloat( rgb.z ) );
}

Rgba8 Rgba8::Lerp( const Rgba8& from, const Rgba8& to, const float percentage )
{
    Rgba8 lerp;
    lerp.r = static_cast<unsigned char>(InterpolateAndClamp( from.r, to.r, percentage ));
    lerp.g = static_cast<unsigned char>(InterpolateAndClamp( from.g, to.g, percentage ));
    lerp.b = static_cast<unsigned char>(InterpolateAndClamp( from.b, to.b, percentage ));
    lerp.a = static_cast<unsigned char>(InterpolateAndClamp( from.a, to.a, percentage ));
    return lerp;
}

Rgba8 Rgba8::LerpAsHSL( const Rgba8& from, const Rgba8& to, const float percentage )
{
    const Vec3 fromHSL = ConvertRgbToHSL( from );
    const Vec3 toHSL = ConvertRgbToHSL( to );
    const Vec3 lerp = Vec3::Lerp( fromHSL, toHSL, percentage );
    return ConvertHSLToRgb( lerp );
}

//-----------------------------------------------------------------------------
STATIC bool Rgba8::CompareRGB( const Rgba8& color1, const Rgba8 color2 )
{
    return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b;
}

//-----------------------------------------------------------------------------
float Rgba8::GetRedAsFloat() const
{
    return static_cast<float>(r) / 255.f;
}

//-----------------------------------------------------------------------------
float Rgba8::GetGreenAsFloat() const
{
    return static_cast<float>(g) / 255.f;
}

//-----------------------------------------------------------------------------
float Rgba8::GetBlueAsFloat() const
{
    return static_cast<float>(b) / 255.f;
}

//-----------------------------------------------------------------------------
float Rgba8::GetAlphaAsFloat() const
{
    return static_cast<float>(a) / 255.f;
}

Vec4 Rgba8::GetAsVec4() const
{
    return Vec4( GetRedAsFloat(), GetGreenAsFloat(), GetBlueAsFloat(), GetAlphaAsFloat() );
}

const Rgba8 Rgba8::GetScaledColor( float scale ) const
{
    Rgba8 newColor( r, g, b );
    newColor.r = MultiplyByFloat( r, scale );
    newColor.g = MultiplyByFloat( g, scale );
    newColor.b = MultiplyByFloat( b, scale );
    return newColor;
}

void Rgba8::ScaleAlpha( float alphaMultiplier )
{
    a = MultiplyByFloat( a, alphaMultiplier );
}

void Rgba8::SetAlphaAsPercent( float percent )
{
    a = static_cast<unsigned char>(Interpolate( 0.f, 255.f, percent ));
}

void Rgba8::Randomize( RandomNumberGenerator& rng )
{
    r = rng.RandomByte();
    g = rng.RandomByte();
    b = rng.RandomByte();
    a = rng.RandomByte();
}

void Rgba8::RandomizeColor( RandomNumberGenerator& rng )
{
    r = rng.RandomByte();
    g = rng.RandomByte();
    b = rng.RandomByte();
}

bool Rgba8::operator==( const Rgba8& rhs ) const
{
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}

// Using 255.f * 255.f in the clamp to simulate dividing by 255.f for r, g, b. Therefore
//  255 should only be returned if both are 255
static constexpr float INVERSE_MAX = 1.f / 255.f;

Rgba8 Rgba8::operator*( const Rgba8& rhs ) const
{
    Rgba8 newColor;
    newColor.r = static_cast<unsigned char>(Clamp(
                                                  static_cast<float>(this->r) * static_cast<float>(rhs.r),
                                                  0.f, 255.f * 255.f
                                                 ) * INVERSE_MAX);
    newColor.g = static_cast<unsigned char>(Clamp(
                                                  static_cast<float>(this->g) * static_cast<float>(rhs.g),
                                                  0.f, 255.f * 255.f
                                                 ) * INVERSE_MAX);
    newColor.b = static_cast<unsigned char>(Clamp(
                                                  static_cast<float>(this->b) * static_cast<float>(rhs.b),
                                                  0.f, 255.f * 255.f
                                                 ) * INVERSE_MAX);
    newColor.a = static_cast<unsigned char>(Clamp(
                                                  static_cast<float>(this->a) * static_cast<float>(rhs.a),
                                                  0.f, 255.f * 255.f
                                                 ) * INVERSE_MAX);
    return newColor;
}

unsigned char Rgba8::MultiplyByFloat( unsigned char value, float f ) const
{
    return static_cast<unsigned char>(Clamp( static_cast<float>(value) * f,
                                             0.f,
                                             255.f
                                           ));
}

STATIC Rgba8 Rgba8::RED = Rgba8( 255, 0, 0 );
STATIC Rgba8 Rgba8::DARK_RED = Rgba8( 100, 0, 0 );
STATIC Rgba8 Rgba8::GREEN = Rgba8( 0, 255, 0 );
STATIC Rgba8 Rgba8::DARK_GREEN = Rgba8( 0, 100, 0 );
STATIC Rgba8 Rgba8::BLUE = Rgba8( 0, 0, 255 );
STATIC Rgba8 Rgba8::DARK_BLUE = Rgba8( 0, 0, 100 );
STATIC Rgba8 Rgba8::WHITE = Rgba8( 255, 255, 255 );
STATIC Rgba8 Rgba8::LIGHT_GRAY = Rgba8( 200, 200, 200 );
STATIC Rgba8 Rgba8::GRAY = Rgba8( 150, 150, 150 );
STATIC Rgba8 Rgba8::MEDIUM_GRAY = Rgba8( 100, 100, 100 );
STATIC Rgba8 Rgba8::DARK_GRAY = Rgba8( 50, 50, 50 );
STATIC Rgba8 Rgba8::BLACK = Rgba8( 0, 0, 0 );
STATIC Rgba8 Rgba8::YELLOW = Rgba8( 255, 255, 0 );
STATIC Rgba8 Rgba8::DARK_YELLOW = Rgba8( 100, 100, 0 );
STATIC Rgba8 Rgba8::MAGENTA = Rgba8( 255, 0, 255 );
STATIC Rgba8 Rgba8::DARK_MAGENTA = Rgba8( 100, 0, 100 );
STATIC Rgba8 Rgba8::CYAN = Rgba8( 0, 255, 255 );
STATIC Rgba8 Rgba8::DARK_CYAN = Rgba8( 0, 100, 100 );

STATIC Rgba8 Rgba8::TAN = Rgba8( 210, 180, 140 );

STATIC Rgba8 Rgba8::RED_75 = Rgba8( 255, 0, 0, 191 );
STATIC Rgba8 Rgba8::RED_50 = Rgba8( 255, 0, 0, 127 );
STATIC Rgba8 Rgba8::RED_25 = Rgba8( 255, 0, 0, 64 );
STATIC Rgba8 Rgba8::DARK_RED_75 = Rgba8( 100, 0, 0, 191 );
STATIC Rgba8 Rgba8::DARK_RED_50 = Rgba8( 100, 0, 0, 127 );
STATIC Rgba8 Rgba8::DARK_RED_25 = Rgba8( 100, 0, 0, 64 );
STATIC Rgba8 Rgba8::GREEN_75 = Rgba8( 0, 255, 0, 191 );
STATIC Rgba8 Rgba8::GREEN_50 = Rgba8( 0, 255, 0, 127 );
STATIC Rgba8 Rgba8::GREEN_25 = Rgba8( 0, 255, 0, 64 );
STATIC Rgba8 Rgba8::DARK_GREEN_75 = Rgba8( 0, 100, 0, 191 );
STATIC Rgba8 Rgba8::DARK_GREEN_50 = Rgba8( 0, 100, 0, 127 );
STATIC Rgba8 Rgba8::DARK_GREEN_25 = Rgba8( 0, 100, 0, 64 );
STATIC Rgba8 Rgba8::BLUE_75 = Rgba8( 0, 0, 255, 191 );
STATIC Rgba8 Rgba8::BLUE_50 = Rgba8( 0, 0, 255, 127 );
STATIC Rgba8 Rgba8::BLUE_25 = Rgba8( 0, 0, 255, 64 );
STATIC Rgba8 Rgba8::DARK_BLUE_75 = Rgba8( 0, 0, 100, 191 );
STATIC Rgba8 Rgba8::DARK_BLUE_50 = Rgba8( 0, 0, 100, 127 );
STATIC Rgba8 Rgba8::DARK_BLUE_25 = Rgba8( 0, 0, 100, 64 );
STATIC Rgba8 Rgba8::WHITE_75 = Rgba8( 255, 255, 255, 191 );
STATIC Rgba8 Rgba8::WHITE_50 = Rgba8( 255, 255, 255, 127 );
STATIC Rgba8 Rgba8::WHITE_25 = Rgba8( 255, 255, 255, 64 );
STATIC Rgba8 Rgba8::LIGHT_GRAY_75 = Rgba8( 200, 200, 200, 191 );
STATIC Rgba8 Rgba8::LIGHT_GRAY_50 = Rgba8( 200, 200, 200, 127 );
STATIC Rgba8 Rgba8::LIGHT_GRAY_25 = Rgba8( 200, 200, 200, 64 );
STATIC Rgba8 Rgba8::GRAY_75 = Rgba8( 150, 150, 150, 191 );
STATIC Rgba8 Rgba8::GRAY_50 = Rgba8( 150, 150, 150, 127 );
STATIC Rgba8 Rgba8::GRAY_25 = Rgba8( 150, 150, 150, 64 );
STATIC Rgba8 Rgba8::MEDIUM_GRAY_75 = Rgba8( 100, 100, 100, 191 );
STATIC Rgba8 Rgba8::MEDIUM_GRAY_50 = Rgba8( 100, 100, 100, 127 );
STATIC Rgba8 Rgba8::MEDIUM_GRAY_25 = Rgba8( 100, 100, 100, 64 );
STATIC Rgba8 Rgba8::DARK_GRAY_75 = Rgba8( 50, 50, 50, 191 );
STATIC Rgba8 Rgba8::DARK_GRAY_50 = Rgba8( 50, 50, 50, 127 );
STATIC Rgba8 Rgba8::DARK_GRAY_25 = Rgba8( 50, 50, 50, 64 );
STATIC Rgba8 Rgba8::BLACK_75 = Rgba8( 0, 0, 0, 191 );
STATIC Rgba8 Rgba8::BLACK_50 = Rgba8( 0, 0, 0, 127 );
STATIC Rgba8 Rgba8::BLACK_25 = Rgba8( 0, 0, 0, 64 );
STATIC Rgba8 Rgba8::YELLOW_75 = Rgba8( 255, 255, 0, 191 );
STATIC Rgba8 Rgba8::YELLOW_50 = Rgba8( 255, 255, 0, 127 );
STATIC Rgba8 Rgba8::YELLOW_25 = Rgba8( 255, 255, 0, 64 );
STATIC Rgba8 Rgba8::DARK_YELLOW_75 = Rgba8( 100, 100, 0, 191 );
STATIC Rgba8 Rgba8::DARK_YELLOW_50 = Rgba8( 100, 100, 0, 127 );
STATIC Rgba8 Rgba8::DARK_YELLOW_25 = Rgba8( 100, 100, 0, 64 );
STATIC Rgba8 Rgba8::MAGENTA_75 = Rgba8( 255, 0, 255, 191 );
STATIC Rgba8 Rgba8::MAGENTA_50 = Rgba8( 255, 0, 255, 127 );
STATIC Rgba8 Rgba8::MAGENTA_25 = Rgba8( 255, 0, 255, 64 );
STATIC Rgba8 Rgba8::DARK_MAGENTA_75 = Rgba8( 100, 0, 100, 191 );
STATIC Rgba8 Rgba8::DARK_MAGENTA_50 = Rgba8( 100, 0, 100, 127 );
STATIC Rgba8 Rgba8::DARK_MAGENTA_25 = Rgba8( 100, 0, 100, 64 );
STATIC Rgba8 Rgba8::CYAN_75 = Rgba8( 0, 255, 255, 191 );
STATIC Rgba8 Rgba8::CYAN_50 = Rgba8( 0, 255, 255, 127 );
STATIC Rgba8 Rgba8::CYAN_25 = Rgba8( 0, 255, 255, 64 );
STATIC Rgba8 Rgba8::DARK_CYAN_75 = Rgba8( 0, 100, 100, 191 );
STATIC Rgba8 Rgba8::DARK_CYAN_50 = Rgba8( 0, 100, 100, 127 );
STATIC Rgba8 Rgba8::DARK_CYAN_25 = Rgba8( 0, 100, 100, 64 );

STATIC Rgba8 Rgba8::CLEAR = Rgba8( 0, 0, 0, 0 );

std::string ConvertToString( const Rgba8& value )
{
    return Stringf( "%hhu,%hhu,%hhu,%hhu", value.r, value.g, value.b, value.a );;
}

std::string GetTypeName( const Rgba8& value )
{
    UNUSED( value );


    return std::string( "Rgba8" );
}

/* Convert Rgba8 from a formatted string
 - Format (r,g,b,a) or r,g,b,a
 */
Rgba8 ConvertFromString( const char* string, const Rgba8& defaultValue )
{
    return ConvertFromString( std::string( string ), defaultValue );
}

/* Convert Rgba8 from a formatted string
 - Format (r,g,b,a) or r,g,b,a
 */
Rgba8 ConvertFromString( const std::string& string, const Rgba8& defaultValue )
{
    Rgba8 result = defaultValue;
    const Strings split = SplitStringOnDelimiter( string, ',' );
    if( split.size() == 4 )
    {
        if( split.at( 0 ).front() == '(' )
        {
            result.r = ConvertFromString( split.at( 0 ).substr( 1 ), defaultValue.r );

            const size_t count = split.at( 2 ).size() - 1;
            result.a = ConvertFromString( split.at( 1 ).substr( 0, count ), defaultValue.a );
        }
        else
        {
            result.r = ConvertFromString( split.at( 0 ), defaultValue.r );
            result.a = ConvertFromString( split.at( 2 ), defaultValue.a );
        }

        // Y and Z does not have any special case
        result.g = ConvertFromString( split.at( 1 ), defaultValue.g );
        result.b = ConvertFromString( split.at( 1 ), defaultValue.b );
    }
    return result;
}
