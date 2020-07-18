#pragma once

#include <vector>
#include <functional>

template <typename ...ArgumentTypes>
class Delegate
{
public:
    using FunctionType = std::function<void( ArgumentTypes... )>;
    using CallbackType = void(*)(ArgumentTypes...);

    struct Subscription
    {
        void const* objectId = nullptr;
        void const* functionId = nullptr;
        FunctionType callable;

        inline bool operator==( const Subscription& rhs) const
        {
            return objectId == rhs.objectId && functionId == rhs.functionId;
        }
    };

    void Subscribe( const CallbackType& callback );
    void Unsubscribe( const CallbackType& callback );
    size_t GetSubscriptionNumber() const { return m_Callbacks.size(); }
    bool Empty() const { return m_Callbacks.empty(); }

    template <typename ObjectType>
    void Subscribe( ObjectType* object, void (ObjectType::* methodCallback)(ArgumentTypes ...) );
    template <typename ObjectType>
    void Unsubscribe( ObjectType* object, void (ObjectType::* methodCallback)(ArgumentTypes ...) );

    template <typename Lambda>
    void Subscribe( size_t lambdaCategory, const Lambda& lambda );
    void Unsubscribe( size_t lambdaCategory );
    
    void Invoke( const ArgumentTypes&... args );

    void operator+=( const CallbackType& callback );
    void operator-=( const CallbackType& callback );
    void operator()( const ArgumentTypes&... args );

private:
    void Subscribe( const Subscription& sub );
    void Unsubscribe( const Subscription& sub, bool compareFunction = true );

    std::vector<Subscription> m_Callbacks;
};

template <typename ...ArgumentTypes>
inline void Delegate<ArgumentTypes...>::Subscribe( const CallbackType& callback )
{
    Subscription sub;
    sub.functionId = callback;
    sub.callable = callback;
    
    Subscribe( sub );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::Unsubscribe( const CallbackType& callback )
{
    Subscription sub;
    sub.functionId = callback;

    Unsubscribe( sub );
}

template<typename ...ArgumentTypes>
template<typename ObjectType>
void Delegate<ArgumentTypes...>::Subscribe( ObjectType* object, void(ObjectType::* methodCallback)(ArgumentTypes...) )
{
    Subscription sub;
    sub.objectId = object;
    sub.functionId = *(void const**) &methodCallback;

    sub.callable = [=]( ArgumentTypes...args ) { (object->*methodCallback)(args...); };
    Subscribe( sub );
}

template<typename ...ArgumentTypes>
template<typename ObjectType>
void Delegate<ArgumentTypes...>::Unsubscribe( ObjectType* object, void(ObjectType::* methodCallback)(ArgumentTypes...) )
{
    Subscription sub;
    sub.objectId = object;
    sub.functionId = *(void**) &methodCallback;

    Unsubscribe( sub );
}

template<typename ...ArgumentTypes>
template<typename Lambda>
void Delegate<ArgumentTypes...>::Subscribe( size_t lambdaCategory, const Lambda& lambda )
{
    Subscription sub;
    sub.objectId = (void*)lambdaCategory;
    sub.functionId = nullptr;

    sub.callable = lambda;

    Subscribe( sub );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::Unsubscribe( size_t lambdaCategory )
{
    Subscription sub;
    sub.objectId = (void*)lambdaCategory;
    sub.functionId = nullptr;

    Unsubscribe( sub );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::Invoke( const ArgumentTypes& ...args )
{
    for ( Subscription& subscription : m_Callbacks )
    {
        subscription.callable( args... );
    }
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::operator+=( const CallbackType& callback )
{
    Subscribe( callback );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::operator-=( const CallbackType& callback )
{
    Unsubscribe( callback );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::operator()( const ArgumentTypes& ...args )
{
    Invoke( args... );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::Subscribe( const Subscription& sub )
{
    m_Callbacks.push_back( sub );
}

template<typename ...ArgumentTypes>
void Delegate<ArgumentTypes...>::Unsubscribe( const Subscription& sub, const bool compareFunction )
{
    for ( size_t index = 0; index < m_Callbacks.size(); ++index )
    {
        if ( compareFunction && m_Callbacks.at( index ) == sub || 
             !compareFunction && m_Callbacks.at( index ).objectId == sub.objectId )
        {
            m_Callbacks[ index ] = m_Callbacks.at( m_Callbacks.size() - 1 );
            m_Callbacks.pop_back();

            index -= 1;
        }
    }
}


