#pragma once

#include "Utils/TypePropertyUtils.hpp"

#include <string>
#include <map>
#include <vector>

class NamedProperties
{
public:
    NamedProperties() = default;
    NamedProperties( const NamedStrings& strings, const std::map<std::string, TypePropertiesBase*>& types );

    template <typename Value>
    void SetValue( const std::string& key, Value value );

    std::string GetValue( const std::string& key, const char* defaultValue );
    std::string GetValue( const std::vector<std::string>& keys, const char* defaultValue );

    template <typename Value>
    Value GetValue( const std::string& key, Value defaultValue );
    template <typename  Value>
    Value GetValue( const std::vector<std::string>& keys, Value defaultValue );
    template <typename Value>
    Value* GetValue( const std::string& key, Value* defaultValue );

    size_t GetSize() const;

private:
    TypePropertiesBase* FindInMap( const std::string& key );

    std::map<std::string, TypePropertiesBase*> m_NamedProperties;
};

template<typename Value>
void NamedProperties::SetValue( const std::string& key, Value value )
{
    TypePropertiesBase* found = FindInMap( key );
    TypeProperties<Value>* foundValue = nullptr;
    if( found != nullptr )
    {
        if( !found->Is<Value>() )
        {
            delete found;
            foundValue = new TypeProperties<Value>();
        }
        else
        {
            foundValue = (TypeProperties<Value>*)found;
        }
    }
    else
    {
        foundValue = new TypeProperties<Value>();
    }

    foundValue->m_Value = value;
    m_NamedProperties[ key ] = foundValue;
}

template<typename Value>
Value NamedProperties::GetValue( const std::string& key, Value defaultValue )
{
    TypePropertiesBase* found = FindInMap( key );
    if( found != nullptr )
    {

        if( found->Is<Value>() )
        {
            const TypeProperties<Value>* foundValue = (TypeProperties<Value>*)found;
            return foundValue->m_Value;
        }

        return ConvertFromString( found->TypeName(), defaultValue );
    }

    return defaultValue;
}

template<typename Value>
inline Value NamedProperties::GetValue( const std::vector<std::string>& keys, Value defaultValue )
{
    for( const std::string& key : keys )
    {
        TypePropertiesBase* found = FindInMap( key );
        if ( found != nullptr )
        {

            if ( found->Is<Value>() )
            {
                const TypeProperties<Value>* foundValue = (TypeProperties<Value>*)found;
                return foundValue->m_Value;
            }

            return ConvertFromString( found->ToString(), defaultValue );
        }
    }

    return defaultValue;
}

template<typename Value>
Value* NamedProperties::GetValue( const std::string& key, Value* defaultValue )
{
    TypePropertiesBase* found = FindInMap( key );
    if ( found != nullptr )
    {

        if ( found->Is<Value*>() )
        {
            const TypeProperties<Value*>* foundValue = (TypeProperties<Value*>*)found;
            return foundValue->m_Value;
        }
    }

    return defaultValue;
}
