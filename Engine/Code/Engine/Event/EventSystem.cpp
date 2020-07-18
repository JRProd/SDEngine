#include "EventSystem.hpp"

#include "Engine/Console/Console.hpp"

//-----------------------------------------------------------------------------------------------
// To disable event system entirely (and remove requirement to link global console) for any game,
//	#define ENGINE_DISABLE_EVENT_SYSTEM in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
#if !defined(ENGINE_DISABLE_EVENT_SYSTEM)
#include "Engine/Core/EngineCommon.hpp"

EXTERNED EventSystem* g_EventSystem = nullptr;

EventSystem::EventSystem()
{
}

void EventSystem::Subscribe( EventName eventName,
                             EventCallback* callbackFunction )
{
#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_VERBOSE, Stringf( "%s registered a new subscriber %p", 
                                          eventName.c_str(), callbackFunction ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)

    // Find or create new list.
    // Add callback to list
    m_EventSubscriptions[ eventName ].Subscribe( callbackFunction );
}

void EventSystem::SubscribeUnique( EventName eventName, EventCallback* callbackFunction )
{
    auto foundEvent = m_EventSubscriptions.find( eventName );
    if( foundEvent == m_EventSubscriptions.cend() )
    {
        // m_EventSubscriptions.emplace( eventName, DelegateWithConsume<EventArgs*>() );
        m_EventSubscriptions[ eventName ].Subscribe( callbackFunction );
    }
    else if (foundEvent->second.Empty() )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_VERBOSE, Stringf( "%s registered a new subscriber %p",
                                              eventName.c_str(), callbackFunction ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)

        // Find or create new list.
        // Add callback to list
        m_EventSubscriptions[ eventName ].Subscribe( callbackFunction );
    }
    else
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "%s is allready registered", eventName.c_str() ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE
    }
}

void EventSystem::Unsubscribe( EventName eventName, 
                               EventCallback* callbackFunction )
{
#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_VERBOSE, Stringf( "%p unsubscribed from %s",
                                          callbackFunction, eventName.c_str() ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)

    EventSubscriptions::iterator foundEvent = m_EventSubscriptions.find( eventName );
    if ( foundEvent != m_EventSubscriptions.end() )
    {
        DelegateWithConsume<EventArgs*>& subscribedEvent = foundEvent->second;
        subscribedEvent.Unsubscribe( callbackFunction );
    }
}

void EventSystem::Publish( const EventName eventName, EventArgs* args ) const
{
#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_VERBOSE, Stringf( "Published %s", eventName.c_str() ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)

    EventSubscriptions::const_iterator foundEvent = m_EventSubscriptions.cbegin();
    for ( ; foundEvent != m_EventSubscriptions.cend(); ++foundEvent )
    {
        if ( Stricmp( eventName, foundEvent->first ) == 0 )
        {
            const DelegateWithConsume<EventArgs*> subscribedEvent = foundEvent->second;
            subscribedEvent( args );
        }
    }
}
#endif // !defined(ENGINE_DISABLE_EVENT_SYSTEM)