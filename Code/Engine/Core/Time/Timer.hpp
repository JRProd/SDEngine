#pragma once

class Clock;

class Timer
{
public:
    Timer() = default;
    Timer( Clock* clock, const double timeToWait );

    void Set( const double timeToWait );
    void Set( Clock* clock, const double timeToWait );
    void SetDuration( const double timeToWait );

    bool IsRunning() const;
    bool HasElapsed() const;
    bool CheckAndDecrement();
    unsigned int CheckAndDecrementAll();
    bool CheckAndReset();

    float GetPercentage() const;
    double GetIncrement() const { return m_DurationSeconds; }

    void Stop();
    float Lap();
    void Reset();

private:
    Clock* m_Clock = nullptr;

    double m_TimerStart = 0.f;
    double m_DurationSeconds = -1.f;
};
