#pragma once

#include <vector>
#include <functional>

template <typename ...ArgumentTypes>
class DelegateWithConsume
{
public:
    using FunctionType = std::function<bool( ArgumentTypes... )>;
    using CallbackType = bool(*)(ArgumentTypes...);

    struct Subscription
    {
        void const* objectId = nullptr;
        void const* functionId = nullptr;
        FunctionType callable;

        inline bool operator==( const Subscription& rhs ) const
        {
            return objectId == rhs.objectId && functionId == rhs.functionId;
        }
    };

    DelegateWithConsume();

    void Subscribe( const CallbackType& callback );
    void Unsubscribe( const CallbackType& callback );
    size_t GetSubscriptionNumber() const { return m_Callbacks.size(); }
    bool Empty() const { return m_Callbacks.empty(); }

    template <typename ObjectType>
    void Subscribe( ObjectType* object, bool (ObjectType::* methodCallback)(ArgumentTypes ...) );
    template<typename ObjectType>
    void Unsubscribe( ObjectType* object );
    template <typename ObjectType>
    void Unsubscribe( ObjectType* object, bool (ObjectType::* methodCallback)(ArgumentTypes ...) );

    template <typename Lambda>
    void Subscribe( size_t lambdaCategory, const Lambda& lambda );
    void Unsubscribe( size_t lambdaCategory );

    void Invoke( const ArgumentTypes&... args ) const;

    void operator+=( const CallbackType& callback );
    void operator-=( const CallbackType& callback );
    void operator()( const ArgumentTypes&... args ) const;

private:
    void Subscribe( const Subscription& sub );
    void Unsubscribe( const Subscription& sub, bool compareFunction = true );

    std::vector<Subscription> m_Callbacks;
};

template<typename ...ArgumentTypes>
DelegateWithConsume<ArgumentTypes...>::DelegateWithConsume()
{
}

template <typename ...ArgumentTypes>
inline void DelegateWithConsume<ArgumentTypes...>::Subscribe( const CallbackType& callback )
{
    Subscription sub;
    sub.functionId = callback;
    sub.callable = callback;

    Subscribe( sub );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::Unsubscribe( const CallbackType& callback )
{
    Subscription sub;
    sub.functionId = callback;

    Unsubscribe( sub );
}

template<typename ...ArgumentTypes>
template<typename ObjectType>
void DelegateWithConsume<ArgumentTypes...>::Subscribe( ObjectType* object, bool(ObjectType::* methodCallback)(ArgumentTypes...) )
{
    Subscription sub;
    sub.objectId = object;
    sub.functionId = *(void const**) &methodCallback;

    sub.callable = [ = ]( ArgumentTypes...args )->bool { return (object->*methodCallback)(args...); };
    Subscribe( sub );
}

template<typename ...ArgumentTypes>
template<typename ObjectType>
void DelegateWithConsume<ArgumentTypes...>::Unsubscribe( ObjectType* object )
{
    Subscription sub;
    sub.objectId = object;
    sub.functionId = nullptr;

    Unsubscribe( sub, false );
}

template<typename ...ArgumentTypes>
template<typename ObjectType>
void DelegateWithConsume<ArgumentTypes...>::Unsubscribe( ObjectType* object, bool(ObjectType::* methodCallback)(ArgumentTypes...) )
{
    Subscription sub;
    sub.objectId = object;
    sub.functionId = *(void**) &methodCallback;

    Unsubscribe( sub );
}

template<typename ...ArgumentTypes>
template<typename Lambda>
void DelegateWithConsume<ArgumentTypes...>::Subscribe( size_t lambdaCategory, const Lambda& lambda )
{
    Subscription sub;
    sub.objectId = (void*) lambdaCategory;
    sub.functionId = nullptr;

    sub.callable = lambda;

    Subscribe( sub );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::Unsubscribe( size_t lambdaCategory )
{
    Subscription sub;
    sub.objectId = (void*) lambdaCategory;
    sub.functionId = nullptr;

    Unsubscribe( sub );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::Invoke( const ArgumentTypes& ...args ) const
{
    for ( const Subscription& subscription : m_Callbacks )
    {
        const bool consume = subscription.callable( args... );
        if( consume )
        {
            return;
        }
    }
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::operator+=( const CallbackType& callback )
{
    Subscribe( callback );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::operator-=( const CallbackType& callback )
{
    Unsubscribe( callback );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::operator()( const ArgumentTypes& ...args ) const
{
    Invoke( args... );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::Subscribe( const Subscription& sub )
{
    m_Callbacks.push_back( sub );
}

template<typename ...ArgumentTypes>
void DelegateWithConsume<ArgumentTypes...>::Unsubscribe( const Subscription& sub, const bool compareFunction )
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


