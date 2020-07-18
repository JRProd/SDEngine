#pragma once

#include <vector>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Renderer/Sprite/SpriteDefinition.hpp"

struct IntVec2;
class Texture;

class SpriteSheet
{
public:
    explicit SpriteSheet( const Texture& texture, const IntVec2& gridLayout, bool flipV = false );

    const SpriteDefinition& GetSpriteDefinition( int spriteIndex ) const;
    const Texture& GetTexture() const { return m_Texture; }
    const IntVec2 GetSpriteSheetSize() const { return m_SpriteSheetSize; }
    void GetSpriteUVs( int spriteIndex, OUT_PARAM Vec2& uvAtBottomLeft, OUT_PARAM Vec2& uvAtTopRight ) const;
    void GetSpriteUVs( const IntVec2& spriteIndex, OUT_PARAM Vec2& uvAtBottomLeft, OUT_PARAM Vec2& uvAtTopRight ) const;
    int GetNumberOfSprites() const { return ( int) m_Definitions.size(); }

private:
    const Texture& m_Texture;

    std::vector<SpriteDefinition> m_Definitions;
    IntVec2 m_SpriteSheetSize = IntVec2::ZERO;
};