#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"

class SpriteSheet;
class Texture;

class SpriteDefinition
{
public:
    ~SpriteDefinition() {}
    explicit SpriteDefinition( const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtBottomLeft, const Vec2& uvAtTopRight );

    int GetSpriteIndex() const { return m_SpriteIndex; }
    const SpriteSheet& GetSpriteSheet() const { return m_SpriteSheet; }
    const Texture& GetTexture() const;
    float GetAspectRatio() const;

    void GetUVs( OUT_PARAM Vec2& uvAtBottomLeft, OUT_PARAM Vec2& uvAtTopRight ) const;

private:
    const SpriteSheet& m_SpriteSheet;
    int m_SpriteIndex = -1;

    Vec2 m_UVAtBottomLeft = Vec2::ZERO;
    Vec2 m_UVAtTopRight = Vec2::ONE;
};