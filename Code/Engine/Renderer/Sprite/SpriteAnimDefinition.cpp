#include "SpriteAnimDefinition.hpp"

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet,
                                            int startSpriteIndex,
                                            int endSpriteIndex,
                                            float durationSeconds,
                                            SpriteAnimPlaybackType animPlayback )
    : m_SpriteSheet( sheet )
    , m_Duration( durationSeconds )
    , m_PlaybackType( animPlayback )
{
    if ( startSpriteIndex == endSpriteIndex )
    {
        m_AnimationFrames.push_back( &sheet.GetSpriteDefinition( startSpriteIndex ) );
        m_AnimationFrames.push_back( &sheet.GetSpriteDefinition( endSpriteIndex ) );
    }
    else if ( startSpriteIndex < endSpriteIndex )
    {
        for ( int spriteIndex = startSpriteIndex; spriteIndex <= endSpriteIndex; ++spriteIndex )
        {
            m_AnimationFrames.push_back( &sheet.GetSpriteDefinition( spriteIndex ) );
        }
    }
    else // startSpriteIndex > endSpriteIndex
    {
        for ( int spriteIndex = endSpriteIndex; spriteIndex >= startSpriteIndex; --spriteIndex )
        {
            m_AnimationFrames.push_back( &sheet.GetSpriteDefinition( spriteIndex ) );
        }
    }
}

SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet,
                                            const std::vector<int>& spriteIndexes,
                                            float durationSeconds,
                                            SpriteAnimPlaybackType animPlayback )
    : m_SpriteSheet( sheet )
    , m_Duration( durationSeconds )
    , m_PlaybackType( animPlayback )
{
    for ( int spriteIndex : spriteIndexes )
    {
        m_AnimationFrames.push_back( &m_SpriteSheet.GetSpriteDefinition( spriteIndex ) );
    }
}

bool SpriteAnimDefinition::IsComplete( float seconds ) const
{
    return m_PlaybackType == SpriteAnimPlaybackType::ONCE && seconds > m_Duration;
}

const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
    switch ( m_PlaybackType )
    {
        case SpriteAnimPlaybackType::ONCE:
            return *m_AnimationFrames.at( GetOnceIndex( seconds ) );
        case SpriteAnimPlaybackType::LOOP:
            return *m_AnimationFrames.at( GetLoopingIndex( seconds ) );
        case SpriteAnimPlaybackType::PINGPONG:
            return *m_AnimationFrames.at( GetPingPongIndex( seconds ) );
        default:
            return *m_AnimationFrames.at( GetLoopingIndex( seconds ) );
    }
}

void SpriteAnimDefinition::SetFPS( float newFps )
{
    m_Duration = static_cast<float>(m_AnimationFrames.size()) / newFps;
}

void SpriteAnimDefinition::SetDuration( float newDuration )
{
    m_Duration = newDuration;
}

int SpriteAnimDefinition::GetOnceIndex( float seconds ) const
{
    if ( seconds >= m_Duration ) { return static_cast<int>(m_AnimationFrames.size()) - 1; }
    if ( seconds <= 0 ) { return 0; }
    return GetIndexBasedOnDuration( seconds );
}

int SpriteAnimDefinition::GetLoopingIndex( float seconds ) const
{
    int index = GetIndexBasedOnDuration( seconds );
    if ( seconds >= 0 )
    {
        return index;
    }
    else
    {
        int size = static_cast<int>(m_AnimationFrames.size());
        return (size - index) % size;
    }
}

int SpriteAnimDefinition::GetPingPongIndex( float seconds ) const
{
    int currentFrame = GetIndexBasedOnDuration( seconds );
    if ( currentFrame <= static_cast<int>(m_AnimationFrames.size()) - 1 )
    {
        return currentFrame;
    }
    else
    {
        return static_cast<int>(m_AnimationFrames.size()) - currentFrame;
        //return m_EndIndex + m_AnimFrames - currentFrame + 1;
    }

    // Ping pong time is double the duration minus two frame

//     float modSeconds = seconds;
//     if ( modSeconds > pingPongTime )
//     {
//         float integerDivide = floori( modSeconds / pingPongTime );
//         modSeconds -= integerDivide * pingPongTime;
//     }
// 
//     if ( modSeconds < m_Duration )
//     {
//         return GetIndexBasedOnDuration( modSeconds );
//     }
//     // Remove one second of 
//     else if ( modSeconds < pingPongTime );
//     {
//         // Get frames m_EndIndex - 1 up to m_StartIndex + 1
//         //  Don't need to worry about m_StartIndex because next frame is modded
//         //  back down to normal m_StartIndex -> m_EndIndex
//         return m_EndIndex - GetIndexBasedOnDuration( modSeconds - m_Duration + secondsPerFrame );
//     }
}

int SpriteAnimDefinition::GetIndexBasedOnDuration( float seconds ) const
{
    float timePerFrame = m_Duration / m_AnimationFrames.size();
    int frame = floori( seconds / timePerFrame );

    return absMod( frame, static_cast<int>(m_AnimationFrames.size()) );
}
