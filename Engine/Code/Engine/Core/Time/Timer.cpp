#include "Timer.hpp"

#include "Clock.hpp"
#include "Engine/Core/Math/MathUtils.hpp"


Timer::Timer( Clock* clock, const double timeToWait )
{
    Set( clock, timeToWait );
}

void Timer::Set( const double timeToWait )
{
    Set( nullptr, timeToWait );
}

void Timer::Set( Clock* clock, const double timeToWait )
{
    if ( clock == nullptr ) { clock = &Clock::MASTER; }

    m_Clock = clock;
    m_TimerStart = m_Clock->GetTotalElapsedSeconds();
    m_DurationSeconds = (timeToWait > 0) ? timeToWait : -1.f;
}

void Timer::SetDuration( const double timeToWait )
{
    m_DurationSeconds = ( timeToWait > 0 ) ? timeToWait : -1.f;
}

bool Timer::IsRunning() const
{
    return m_DurationSeconds > 0.f;
}

bool Timer::HasElapsed() const
{
    if ( m_DurationSeconds <= 0.f ) { return true; }
    return (m_TimerStart + m_DurationSeconds) < m_Clock->GetTotalElapsedSeconds();
}


bool Timer::CheckAndDecrement()
{
    if ( HasElapsed() )
    {
        m_TimerStart += m_DurationSeconds;
        return true;
    }
    return false;
}

unsigned int Timer::CheckAndDecrementAll()
{
    unsigned int numberOfDecrements = 0;
    while( CheckAndDecrement() )
    {
        numberOfDecrements++;
    }
    return numberOfDecrements;
}

bool Timer::CheckAndReset()
{
    if ( HasElapsed() )
    {
        Reset();
        return true;
    }
    return false;
}

float Timer::GetPercentage() const
{
    if ( HasElapsed() ) { return 1.f; }

    const float percentage = static_cast<float>((m_Clock->GetTotalElapsedSeconds() - m_TimerStart) /
        m_DurationSeconds);
    return ClampZeroToOne( percentage );
}

void Timer::Stop()
{
    m_DurationSeconds = -1.f;
}

float Timer::Lap()
{
    const double nextTime = m_Clock->GetTotalElapsedSeconds();
    const double lap = nextTime - m_TimerStart;
    m_TimerStart = nextTime;
    return static_cast<float>(lap);
}

void Timer::Reset()
{
    m_TimerStart = m_Clock->GetTotalElapsedSeconds();
    m_DurationSeconds = -1.f;
}
