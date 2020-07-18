#include "SpriteDefinition.hpp"

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sprite/SpriteSheet.hpp"

SpriteDefinition::SpriteDefinition( const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtBottomLeft, const Vec2& uvAtTopRight )
    : m_SpriteSheet( spriteSheet )
    , m_SpriteIndex( spriteIndex )
    , m_UVAtBottomLeft( uvAtBottomLeft )
    , m_UVAtTopRight( uvAtTopRight )
{
}

const Texture& SpriteDefinition::GetTexture() const
{
    return m_SpriteSheet.GetTexture();
}

float SpriteDefinition::GetAspectRatio() const
{
    Vec2 sizeOfSprite = Vec2( m_UVAtTopRight.x - m_UVAtBottomLeft.x, m_UVAtBottomLeft.y - m_UVAtTopRight.y );
    return GetTexture().GetAspectRatio() * (sizeOfSprite.x / sizeOfSprite.y);
}

void SpriteDefinition::GetUVs( OUT_PARAM Vec2& uvAtBottomLeft, OUT_PARAM Vec2& uvAtTopRight ) const
{
    uvAtBottomLeft = m_UVAtBottomLeft;
    uvAtTopRight = m_UVAtTopRight;
}
