#include "MaterialSheet.hpp"


#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Renderer/Sprite/SpriteSheet.hpp"

MaterialSheet::MaterialSheet( const IntVec2& gridLayout )
    : m_MaterialSheetSize( gridLayout )
{
}

void MaterialSheet::AddSpriteSheet( const std::string& name, SpriteSheet* sheet )
{
    m_Materials[ name ] = sheet;
}

const Texture& MaterialSheet::GetTexture( const std::string& textureName ) const
{
    auto foundIndex = m_Materials.find( textureName );
    if( foundIndex != m_Materials.cend() )
    {
        return foundIndex->second->GetTexture();
    }

    ERROR_AND_DIE( Stringf("MaterialSheet::GetTexture - No texture found with name %s", textureName.c_str() ) );
}

void MaterialSheet::GetSpriteUVs( int spriteIndex, OUT_PARAM Vec2& uvAtMin, OUT_PARAM Vec2& uvAtMax ) const
{
    m_Materials.begin()->second->GetSpriteUVs( spriteIndex, uvAtMin, uvAtMax );
}

void MaterialSheet::GetSpriteUVs( const IntVec2& spriteIndex, OUT_PARAM Vec2& uvAtMin, OUT_PARAM Vec2& uvAtMax ) const
{
    m_Materials.begin()->second->GetSpriteUVs( spriteIndex, uvAtMin, uvAtMax );
}
