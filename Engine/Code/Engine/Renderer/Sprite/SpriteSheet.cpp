#include "SpriteSheet.hpp"

#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Renderer/Sprite/SpriteDefinition.hpp"

SpriteSheet::SpriteSheet( const Texture& texture, const IntVec2& gridLayout, bool flipV )
    : m_Texture( texture )
  , m_SpriteSheetSize( gridLayout )
{
    const float uvOnX = 1.f / static_cast<float>(m_SpriteSheetSize.x);
    const float uvOnY = 1.f / static_cast<float>(m_SpriteSheetSize.y);

    // Push all of the uvs in by 1% of one pixel to prevent floating point errors
    const Vec2 pushIn = Vec2( uvOnX, uvOnY ) * .01f;

    int numberOfSprites = m_SpriteSheetSize.x * m_SpriteSheetSize.y;

    for( int spriteIndex = 0; spriteIndex < numberOfSprites; ++spriteIndex )
    {
        const float currentXPosition = static_cast<float>(spriteIndex % m_SpriteSheetSize.x);
        const float currentYPosition = static_cast<float>(spriteIndex / m_SpriteSheetSize.x);

        float yUvPosition = uvOnY * currentYPosition;
        float yUvPositionPlusOne = uvOnY * ( currentYPosition + 1 );

        if( flipV )
        {
            yUvPosition = 1.f - yUvPosition;
            yUvPositionPlusOne = 1.f - yUvPositionPlusOne;
        }

        Vec2 uvAtTopLeft( uvOnX * currentXPosition,
                              yUvPosition
                            );
        Vec2 uvAtBottomRight( uvOnX * (currentXPosition + 1),
                          yUvPositionPlusOne
                        );

        if( flipV )
        {
            uvAtTopLeft += pushIn * Vec2(1, -1);
            uvAtBottomRight -= pushIn * Vec2(1, -1);
        }
        else
        {
            uvAtTopLeft += pushIn;
            uvAtBottomRight -= pushIn;
        }

        SpriteDefinition spriteDef( *this, spriteIndex, 
            Vec2(uvAtTopLeft.x, uvAtBottomRight.y), 
            Vec2(uvAtBottomRight.x, uvAtTopLeft.y ) );
        m_Definitions.push_back( spriteDef );
    }
}

const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
    return m_Definitions.at( spriteIndex );
}

void SpriteSheet::GetSpriteUVs( int spriteIndex,
                                OUT_PARAM Vec2& uvAtBottomLeft,
                                OUT_PARAM Vec2& uvAtTopRight ) const
{
    const SpriteDefinition& spriteDef = m_Definitions.at( spriteIndex );
    spriteDef.GetUVs( uvAtBottomLeft, uvAtTopRight );
}

void SpriteSheet::GetSpriteUVs( const IntVec2& spriteIndex, OUT_PARAM Vec2& uvAtBottomLeft,
                                OUT_PARAM Vec2& uvAtTopRight ) const
{
    const int index = spriteIndex.y * m_SpriteSheetSize.x + spriteIndex.x;
    GetSpriteUVs( index, uvAtBottomLeft, uvAtTopRight );
}
