#pragma once

#include "Engine/Renderer/Sprite/SpriteAnimDefinition.hpp"

class RenderContext;

class SpriteAnimSet
{
public:
    SpriteAnimSet( const std::map<const std::string, SpriteAnimDefinition*>& animationSets );

    // Same API as SpriteAnimDefinition with addition of state
    bool IsComplete( const std::string& state, float seconds ) const;
    const SpriteDefinition& GetSpriteDefAtTime( const std::string& state, float seconds ) const;

    std::vector<std::string> GetAnimStates() const;

    void SetSpriteAnimDefinitionsFPS( const std::vector<std::string>& states, float fps ) const;
    void SetSpriteAnimDefinitionsDuration( const std::vector<std::string>& states, float duration ) const;

private:
    std::map<const std::string, SpriteAnimDefinition*> m_AnimationSet;
};