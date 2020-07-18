#pragma once

#include "Engine/Core/Math/Primatives/Mat44.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Renderer/Light/Light.hpp"

struct FrameDataUBO
{
    float systemTime;
    float systemDeltaTime;
    float gameTime;
    float gameDeltaTime;

    float gamma;
    float inverseGamma;
    float pad02[ 2 ];

    bool operator==( const FrameDataUBO& rhs ) const;
    bool operator!=( const FrameDataUBO& rhs ) const;
};

struct CameraDataUBO
{
    Mat44 cameraToClipTransform;
    Mat44 cameraView;

    Vec3 position;
    float pad01 = 0;

    bool operator==( const CameraDataUBO& rhs ) const;
    bool operator!=( const CameraDataUBO& rhs ) const;
};

struct ModelDataUBO
{
    Mat44 meshModel;
    Vec4 meshTint;

    float specularFactor = 1.f;
    float specularPower = 10.f;
    float pad02[ 2 ] = { 0.f };

    bool operator==( const ModelDataUBO& rhs ) const;
    bool operator!=( const ModelDataUBO& rhs ) const;

    ModelDataUBO() = default;
    ModelDataUBO( const Mat44& model, const Vec4& tint, float factor, float power );
};

struct LightDataUBO
{
    Vec4 ambient;
    Light lights[ 8 ];

    bool operator==( const LightDataUBO& rhs ) const;
    bool operator!=( const LightDataUBO& rhs ) const;
};

struct FogDataUBO
{
    Vec4 nearFogColor;
    Vec4 farFogColor;

    float nearFogDistance;
    float farFogDistance;

    float pad02[ 2 ] = { 0.f };

    bool operator==( const FogDataUBO& rhs ) const;
    bool operator!=( const FogDataUBO& rhs ) const;
};