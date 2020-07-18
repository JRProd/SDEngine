#pragma once

#include <string>

#include "Math/Primatives/Vec4.hpp"

struct Rgba8;
class RandomNumberGenerator;

Vec3 ConvertRgbToHSL( const Rgba8& rgba8 );
Rgba8 ConvertHSLToRgb( const Vec3& hsl );

struct Rgba8
{
public:
    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    unsigned char a = 255;

public:
    Rgba8();
    explicit Rgba8( unsigned char r, 
                    unsigned char g, 
                    unsigned char b, 
                    unsigned char a = 255 );
    Rgba8( const Rgba8& copy);

    static Rgba8 CreateFromString( const char* data );
    static Rgba8 CreateFromVec3( const Vec3& rgb );
    static Rgba8 Lerp(  const Rgba8& from, const Rgba8& to, const float percentage );
    static Rgba8 LerpAsHSL(  const Rgba8& from, const Rgba8& to, const float percentage );
    static bool CompareRGB( const Rgba8& color1, const Rgba8 color2 );

    //-------------------------------------------------------------------------
    // Accessors (const)
    float GetRedAsFloat() const;
    float GetGreenAsFloat() const;
    float GetBlueAsFloat() const;
    float GetAlphaAsFloat() const;
    Vec4 GetAsVec4() const;
    const Rgba8 GetScaledColor( float scale ) const;

    //-------------------------------------------------------------------------
    // Mutators (non-const)
    void ScaleAlpha( float scale );
    void SetAlphaAsPercent( float percent );
    void Randomize( RandomNumberGenerator& rng );
    void RandomizeColor( RandomNumberGenerator& rng );

    bool operator==( const Rgba8& rhs ) const;
    Rgba8 operator*( const Rgba8& rhs ) const;

    static Rgba8 RED;
    static Rgba8 DARK_RED;
    static Rgba8 GREEN;
    static Rgba8 DARK_GREEN;
    static Rgba8 BLUE;
    static Rgba8 DARK_BLUE;
    static Rgba8 WHITE;
    static Rgba8 LIGHT_GRAY;
    static Rgba8 GRAY;
    static Rgba8 MEDIUM_GRAY;
    static Rgba8 DARK_GRAY;
    static Rgba8 BLACK;
    static Rgba8 YELLOW;
    static Rgba8 DARK_YELLOW;
    static Rgba8 MAGENTA;
    static Rgba8 DARK_MAGENTA;
    static Rgba8 CYAN;
    static Rgba8 DARK_CYAN;

    static Rgba8 TAN;

    static Rgba8 RED_75;
    static Rgba8 RED_50;
    static Rgba8 RED_25;
    static Rgba8 DARK_RED_75;
    static Rgba8 DARK_RED_50;
    static Rgba8 DARK_RED_25;
    static Rgba8 GREEN_75;
    static Rgba8 GREEN_50;
    static Rgba8 GREEN_25;
    static Rgba8 DARK_GREEN_75;
    static Rgba8 DARK_GREEN_50;
    static Rgba8 DARK_GREEN_25;
    static Rgba8 BLUE_75;
    static Rgba8 BLUE_50;
    static Rgba8 BLUE_25;
    static Rgba8 DARK_BLUE_75;
    static Rgba8 DARK_BLUE_50;
    static Rgba8 DARK_BLUE_25;
    static Rgba8 WHITE_75;
    static Rgba8 WHITE_50;
    static Rgba8 WHITE_25;
    static Rgba8 LIGHT_GRAY_75;
    static Rgba8 LIGHT_GRAY_50;
    static Rgba8 LIGHT_GRAY_25;
    static Rgba8 GRAY_75;
    static Rgba8 GRAY_50;
    static Rgba8 GRAY_25;
    static Rgba8 MEDIUM_GRAY_75;
    static Rgba8 MEDIUM_GRAY_50;
    static Rgba8 MEDIUM_GRAY_25;
    static Rgba8 DARK_GRAY_75;
    static Rgba8 DARK_GRAY_50;
    static Rgba8 DARK_GRAY_25;
    static Rgba8 BLACK_75;
    static Rgba8 BLACK_50;
    static Rgba8 BLACK_25;
    static Rgba8 YELLOW_75;
    static Rgba8 YELLOW_50;
    static Rgba8 YELLOW_25;
    static Rgba8 DARK_YELLOW_75;
    static Rgba8 DARK_YELLOW_50;
    static Rgba8 DARK_YELLOW_25;
    static Rgba8 MAGENTA_75;
    static Rgba8 MAGENTA_50;
    static Rgba8 MAGENTA_25;
    static Rgba8 DARK_MAGENTA_75;
    static Rgba8 DARK_MAGENTA_50;
    static Rgba8 DARK_MAGENTA_25;
    static Rgba8 CYAN_75;
    static Rgba8 CYAN_50;
    static Rgba8 CYAN_25;
    static Rgba8 DARK_CYAN_75;
    static Rgba8 DARK_CYAN_50;
    static Rgba8 DARK_CYAN_25;

    static Rgba8 CLEAR; 

private:
    unsigned char MultiplyByFloat( unsigned char value, float f ) const;
};

std::string ConvertToString( const Rgba8& value );
std::string GetTypeName( const Rgba8& value );
Rgba8 ConvertFromString( const char* string, const Rgba8& defaultValue );
Rgba8 ConvertFromString( const std::string& string, const Rgba8& defaultValue );