#include "Clock.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"

STATIC Clock Clock::MASTER = Clock( nullptr );

Clock::Clock()
    : m_Parent( &Clock::MASTER )
    , m_CreationTime( GetCurrentTimeSeconds() )
    , m_ElapsedSeconds( GetCurrentTimeSeconds() )
{
    if( m_Parent != nullptr )
    {
        m_Parent->AddChild( this );
    }
}

Clock::Clock( Clock* parent )
    : m_Parent( parent )
    , m_CreationTime( GetCurrentTimeSeconds() )
    , m_ElapsedSeconds( GetCurrentTimeSeconds() )
{
    if( m_Parent != nullptr )
    {
        m_Parent->AddChild( this );
    }
}

Clock::~Clock()
{
    for( Clock* child : m_Children )
    {
        if( child != nullptr )
        {
            child->SetParent( m_Parent );
        }
    }
}

STATIC void Clock::BeginFrame()
{
    static double lastFrameTime = GetCurrentTimeSeconds();
    const double currentFrameTime = GetCurrentTimeSeconds();
    const double deltaSeconds = currentFrameTime - lastFrameTime;

    MASTER.Update( deltaSeconds );

    lastFrameTime = currentFrameTime;
}

void Clock::Update( double deltaSeconds )
{
    if ( m_Paused )
    {
        deltaSeconds = 0.f;
    }
    else if( m_MaxDeltaTime > 0.)
    {
        deltaSeconds = Min( deltaSeconds, m_MaxDeltaTime );
    }

    m_DeltaSeconds = deltaSeconds * m_TimeScale;
    m_ElapsedSeconds += m_DeltaSeconds;

    UpdateChildren();
}

void Clock::Reset()
{
    m_CreationTime = GetCurrentTimeSeconds();
    m_ElapsedSeconds = GetCurrentTimeSeconds();
    m_DeltaSeconds = 0.;
    m_TimeScale = 1.;
}

void Clock::Pause()
{
    m_Paused = true;
}

void Clock::Resume()
{
    m_Paused = false;
}

void Clock::SetScale( double scale )
{
    m_TimeScale = scale;
}

void Clock::SetMaxFrameTime( const double maxFrameTime )
{
    ASSERT_OR_DIE( maxFrameTime > 0.f, "Cannot force a maximum time that is zero or less" );
    m_MaxDeltaTime = maxFrameTime;
}

void Clock::SetParent( Clock* parent )
{
    m_Parent = parent;

    if( m_Parent != nullptr )
    {
        m_Parent->AddChild( this );
    }
}

void Clock::AddChild( Clock* child )
{
    m_Children.push_back( child );
}

void Clock::RemoveChild( Clock* child )
{
    for( size_t childIndex = 0; childIndex < m_Children.size(); ++childIndex)
    {
        if( m_Children.at(childIndex) == child)
        {
            m_Children.at( childIndex ) = nullptr;
        }
    }
}

void Clock::UpdateChildren()
{
    for( Clock* child: m_Children )
    {
        if( child != nullptr )
        {
            child->Update( m_DeltaSeconds );
        }
    }
}
