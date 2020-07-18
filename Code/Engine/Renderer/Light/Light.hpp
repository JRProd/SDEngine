#pragma once
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Math/Primatives/Vec4.hpp"

struct Rgba8;

enum class LightType
{
    POINT,
    DIRECTIONAL,
    SPOTLIGHT
};

struct Light
{
    Vec3 position;
    LightType type = LightType::POINT;     // Counts towards a pad

    Vec4 color = Vec4(1,1,1,0);

    Vec3 attenuation = Vec3( 0, 1, 0 );
    float cosHalfInner = -1.f;

    Vec3 specularAttenuation = Vec3( 0, 0, 1 );
    float cosHalfOuter = -1.f;

    Vec3 direction = Vec3(1,0,0);
    float isDirection = 0;

public:
    Light() = default;
    ~Light() = default;

    bool operator==( const Light& rhs ) const;
    bool operator!=( const Light& rhs ) const;
};
