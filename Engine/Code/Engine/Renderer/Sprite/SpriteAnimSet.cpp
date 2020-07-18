#include "SpriteAnimSet.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Renderer/RenderContext.hpp"

SpriteAnimSet::SpriteAnimSet( const std::map<const std::string, SpriteAnimDefinition*>& animationSets )
    : m_AnimationSet( animationSets )
{
}

bool SpriteAnimSet::IsComplete( const std::string& state, float seconds ) const
{
    std::map<const std::string, SpriteAnimDefinition*>::const_iterator currentState = m_AnimationSet.find( state );
    if ( currentState == m_AnimationSet.cend() )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->LogWTF( Stringf( "SpriteAnimSet: No animation state found \"%s\"", state.c_str() ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)
    #if defined(ENGINE_DISABLE_CONSOLE)
        ERROR_AND_DIE( Stringf( "SpriteAnimSet: No animation state found \"%s\"", state.c_str() ) );
    #endif // defined(ENGINE_DISABLE_CONSOLE)
    }

    return currentState->second->IsComplete( seconds );
}

const SpriteDefinition& SpriteAnimSet::GetSpriteDefAtTime( const std::string& state, float seconds ) const
{
    std::map<const std::string, SpriteAnimDefinition*>::const_iterator currentState = m_AnimationSet.find( state );
    if ( currentState == m_AnimationSet.cend() )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->LogWTF( Stringf( "SpriteAnimSet: No animation state found \"%s\"", state.c_str() ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)
    #if defined(ENGINE_DISABLE_CONSOLE)
        ERROR_AND_DIE( Stringf( "SpriteAnimSet: No animation state found \"%s\"", state.c_str() ) );
    #endif // defined(ENGINE_DISABLE_CONSOLE)
    }

    return currentState->second->GetSpriteDefAtTime( seconds );
}

std::vector<std::string> SpriteAnimSet::GetAnimStates() const
{
    std::vector<std::string> states;
    for( auto iterator = m_AnimationSet.cbegin(); iterator != m_AnimationSet.cend(); ++iterator )
    {
        states.push_back( iterator->first );
    }

    return states;
}

void SpriteAnimSet::SetSpriteAnimDefinitionsFPS( const std::vector<std::string>& states, float fps ) const
{
    for( const std::string& state : states )
    {
        auto currentState = m_AnimationSet.find( state );
        if( currentState != m_AnimationSet.cend() )
        {
            currentState->second->SetFPS( fps );
        }
    }
}

void SpriteAnimSet::SetSpriteAnimDefinitionsDuration( const std::vector<std::string>& states, float duration ) const
{
    for( const std::string& state : states )
    {
        auto currentState = m_AnimationSet.find( state );
        if( currentState != m_AnimationSet.cend() )
        {
            currentState->second->SetDuration( duration );
        }
    }
}
