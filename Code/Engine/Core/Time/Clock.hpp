#pragma once

#include <vector>

class Clock
{
public:
    Clock();
    explicit Clock( Clock* parent );
    ~Clock();

    static Clock MASTER;
    static void BeginFrame();

    void Update( double deltaSeconds );
    void Reset();

    //-----------------------------------------------------------------------------
    // Clock Accessor (const)
    double GetTotalElapsedSeconds() const { return m_ElapsedSeconds; }
    float GetTotalElapsedSecondsf() const { return static_cast<float>(m_ElapsedSeconds); }
    double GetSecondsSinceCreation() const { return m_ElapsedSeconds - m_CreationTime; }
    double GetLastDeltaSeconds() const { return m_DeltaSeconds; }
    float GetLastDeltaSecondsf() const { return static_cast<float>(m_DeltaSeconds); }
    double GetScale() const { return m_TimeScale; }
    bool IsPaused() const { return m_Paused; }

    //-----------------------------------------------------------------------------
    // Clock Modifiers
    void Pause();
    void Resume();
    void Toggle() { m_Paused = !m_Paused; }
    void SetScale(double scale);
    void SetMaxFrameTime( const double maxFrameTime );

private:
    Clock* m_Parent;
    std::vector<Clock*> m_Children;

    double m_CreationTime = 0.;
    double m_ElapsedSeconds = 0.;
    double m_DeltaSeconds = 0.;
    double m_MaxDeltaTime = -1.f;
    double m_TimeScale = 1.f;
    bool m_Paused = false;

    void SetParent( Clock* parent );
    void AddChild( Clock* child );
    void RemoveChild( Clock* child );

    void UpdateChildren();
};
