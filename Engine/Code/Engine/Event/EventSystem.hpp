#pragma once

#include "Engine/Core/NamedProperties.hpp"

#include <map>
#include <string>

#include "DelegateWithConsume.hpp"


// Name of event to subscribe to
// typedef std::string
typedef std::string EventName;

// Arguments to pass to an EventCallback
// typedef NamedProperties
typedef NamedProperties EventArgs;

// EventCallback
// bool <FunctionName>( EventArgs& arguments) {}
typedef bool(EventCallback)( EventArgs* arguments );

// List of all event callbacks attributed to a EventName
// typedef std::vector<EventCallback*>

// EventNames and Associated SubscriptionLists
// typedef std::map<EventName, SubscriptionList>
typedef std::map<EventName, DelegateWithConsume<EventArgs*>> EventSubscriptions;


class EventSystem
{
public:
    EventSystem();
    ~EventSystem() = default;

    void Startup() {}
    void Shutdown() {}

    void Subscribe( EventName eventName, EventCallback* callbackFunction );
    void SubscribeUnique( EventName eventName, EventCallback* callbackFunction );
    template <typename ObjectType>
    void Subscribe( EventName eventName, ObjectType* object, bool (ObjectType::* methodCallback)(EventArgs*) );
    template <typename ObjectType>
    void SubscribeUnique( EventName eventName, ObjectType* object, bool (ObjectType:: *methodCallback)(EventArgs*) );
    void Unsubscribe( EventName eventName, EventCallback* callbackFunction );
    template <typename ObjectType>
    void Unsubscribe( ObjectType* object );

    void Publish( EventName eventName, EventArgs* args = nullptr ) const;

private:
    EventSubscriptions m_EventSubscriptions;
};

template<typename ObjectType>
void EventSystem::Subscribe( EventName eventName, ObjectType* object, bool(ObjectType::* methodCallback)(EventArgs*) )
{
    m_EventSubscriptions[ eventName ].Subscribe( object, methodCallback );
}

template<typename ObjectType>
void EventSystem::SubscribeUnique( EventName eventName, ObjectType* object, bool( ObjectType::* methodCallback )( EventArgs* ) )
{

    if( m_EventSubscriptions[eventName].Empty() )
    {
        m_EventSubscriptions[ eventName ].Subscribe( object, methodCallback );
    }
}

template<typename ObjectType>
void EventSystem::Unsubscribe( ObjectType* object )
{
    std::map<EventName, DelegateWithConsume<EventArgs*>>::const_iterator iterator;
    for( iterator = m_EventSubscriptions.cbegin(); iterator != m_EventSubscriptions.cend(); ++iterator )
    {
        m_EventSubscriptions[ iterator->first ].Unsubscribe( object );
    }
}