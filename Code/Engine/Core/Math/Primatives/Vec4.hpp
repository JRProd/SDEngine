#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"

struct Vec4
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;

public:
    // Construction/Destruction
    ~Vec4() = default;											// destructor (do nothing)
    Vec4() = default;											// default constructor (do nothing)
    Vec4( const Vec4& copyFrom );							// copy constructor (from another vec2)
    explicit Vec4( float allFour );
    explicit Vec4( const Vec3& firstThree, float initialW );
    explicit Vec4( float initialX,
                   float initialY,
                   float initialZ,
                   float initialW );		                // explicit constructor (from x, y)

    // Static Vec2s
    static Vec4 ZERO;
    static Vec4 ONE;
    static Vec4 I_BASIS;
    static Vec4 J_BASIS;
    static Vec4 K_BASIS;
    static Vec4 T_BASIS;

    bool operator==( const Vec4& rhs ) const;
    bool operator!=( const Vec4& rhs ) const;

    // Swizzles (const methods)
    Vec2 XX() const { return Vec2( x, x ); }
    Vec2 XY() const { return Vec2( x, y ); }
    Vec2 XZ() const { return Vec2( x, z ); }
    Vec2 XW() const { return Vec2( x, w ); }
    Vec3 XXX() const { return Vec3( x, x, x ); }
    Vec3 XYZ() const { return Vec3( x, y, z ); }
    Vec3 XYW() const { return Vec3( x, y, w ); }
    Vec3 XZY() const { return Vec3( x, z, y ); }
    Vec3 XZW() const { return Vec3( x, z, w ); }
    Vec3 XWY() const { return Vec3( x, w, y ); }
    Vec3 XWZ() const { return Vec3( x, w, z ); }

    Vec2 YY() const { return Vec2( y, y ); }
    Vec2 YX() const { return Vec2( y, x ); }
    Vec2 YZ() const { return Vec2( y, z ); }
    Vec2 YW() const { return Vec2( y, w ); }
    Vec3 YYY() const { return Vec3( y, y, y ); }
    Vec3 YXZ() const { return Vec3( y, x, z ); }
    Vec3 YXW() const { return Vec3( y, x, w ); }
    Vec3 YZX() const { return Vec3( y, z, x ); }
    Vec3 YZW() const { return Vec3( y, z, w ); }
    Vec3 YWX() const { return Vec3( y, w, x ); }
    Vec3 YWZ() const { return Vec3( y, w, z ); }

    Vec2 ZZ() const { return Vec2( z, z ); }
    Vec2 ZX() const { return Vec2( z, x ); }
    Vec2 ZY() const { return Vec2( z, y ); }
    Vec2 ZW() const { return Vec2( z, w ); }
    Vec3 ZZZ() const { return Vec3( z, z, z ); }
    Vec3 ZXY() const { return Vec3( z, x, y ); }
    Vec3 ZXW() const { return Vec3( z, x, w ); }
    Vec3 ZYX() const { return Vec3( z, y, x ); }
    Vec3 ZYW() const { return Vec3( z, y, w ); }
    Vec3 ZWX() const { return Vec3( z, w, x ); }
    Vec3 ZWY() const { return Vec3( z, w, y ); }

    Vec2 WW() const { return Vec2( w, w ); }
    Vec2 WX() const { return Vec2( w, x ); }
    Vec2 WY() const { return Vec2( w, y ); }
    Vec2 WZ() const { return Vec2( w, z ); }
    Vec3 WWW() const { return Vec3( w, w, w ); }
    Vec3 WXY() const { return Vec3( w, x, y ); }
    Vec3 WXZ() const { return Vec3( w, x, z ); }
    Vec3 WYX() const { return Vec3( w, y, x ); }
    Vec3 WYZ() const { return Vec3( w, y, z ); }
    Vec3 WZX() const { return Vec3( w, z, x ); }
    Vec3 WZY() const { return Vec3( w, z, y ); }
};

std::string ConvertToString( const Vec4& value );
std::string GetTypeName( const Vec4& value );
Vec4 ConvertFromString( const char* string, const Vec4& defaultValue );
Vec4 ConvertFromString( const std::string& string, const Vec4& defaultValue );
