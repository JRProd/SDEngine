#pragma once

#include "Engine/Renderer/Sprite/SpriteSheet.hpp"

enum class SpriteAnimPlaybackType
{
    ONCE,
    LOOP,
    PINGPONG,
};

class SpriteAnimDefinition
{
public:
    SpriteAnimDefinition( const SpriteSheet& sheet,
                          int startSpriteIndex,
                          int endSpriteIndex,
                          float durationSeconds,
                          SpriteAnimPlaybackType animPlayback = SpriteAnimPlaybackType::LOOP );

    SpriteAnimDefinition( const SpriteSheet& sheet,
                          const std::vector<int>& spriteIndexes,
                          float durationSeconds,
                          SpriteAnimPlaybackType animPlayback = SpriteAnimPlaybackType::LOOP );

    bool                    IsComplete( float seconds ) const;
    const SpriteDefinition& GetSpriteDefAtTime( float seconds ) const;

    void SetFPS( float newFps );
    void SetDuration( float newDuration );

private:
    const SpriteSheet& m_SpriteSheet;
    const SpriteAnimPlaybackType m_PlaybackType = SpriteAnimPlaybackType::LOOP;
    float m_Duration = 1.f;

    std::vector<const SpriteDefinition*> m_AnimationFrames;

    int GetOnceIndex( float seconds ) const;
    int GetLoopingIndex( float seconds ) const;
    int GetPingPongIndex( float seconds ) const;
    int GetIndexBasedOnDuration( float secondsToDuration ) const;
};