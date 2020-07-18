#pragma once

#include <vector>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Renderer/Sprite/SpriteDefinition.hpp"

struct IntVec2;
class Texture;

class MaterialSheet
{
public:
    explicit MaterialSheet( const IntVec2& gridLayout );
    void AddSpriteSheet( const std::string&, SpriteSheet* sheet );

    const Texture& GetTexture( const std::string& textureName ) const;
    void GetSpriteUVs( int spriteIndex, OUT_PARAM Vec2& uvAtMin, OUT_PARAM Vec2& uvAtMax ) const;
    void GetSpriteUVs( const IntVec2& spriteIndex, OUT_PARAM Vec2& uvAtMin, OUT_PARAM Vec2& uvAtMax ) const;

    IntVec2 GetMaterialSheetSize() const { return m_MaterialSheetSize; }

private:
    std::map<std::string, SpriteSheet*> m_Materials;
    IntVec2 m_MaterialSheetSize = IntVec2::ZERO;
};