#include "NamedProperties.hpp"

NamedProperties::NamedProperties( const NamedStrings& strings, const std::map<std::string, TypePropertiesBase*>& types )
{
    std::map<std::string, TypePropertiesBase*>::const_iterator iterator;
    for( iterator = types.cbegin(); iterator != types.cend(); ++iterator )
    {
        std::string foundValue = strings.GetValue( iterator->first, "" );
        if( !foundValue.empty() )
        {
            TypePropertiesBase* copy = iterator->second->CreateCopy();
            copy->SetFromString( foundValue );

            m_NamedProperties[ iterator->first ] = copy;
        }
    }
}

std::string NamedProperties::GetValue( const std::string& key, const char* defaultValue )
{

    TypePropertiesBase* found = FindInMap( key );
    if( found != nullptr )
    {
        //
        // if( found->Is<const char*>() )
        // {
        //     const TypeProperties<const char*>* foundValue = ( TypeProperties<const char*>* )found;
        //     return foundValue->m_Value;
        // }

        return ConvertToString( ConvertFromString( found->TypeName(), defaultValue ) );
    }

    return defaultValue;
}

std::string NamedProperties::GetValue( const std::vector<std::string>& keys, const char* defaultValue )
{
    for( const std::string& key : keys )
    {
        TypePropertiesBase* found = FindInMap( key );
        if( found != nullptr )
        {
            return ConvertToString( ConvertFromString( found->ToString(), defaultValue ) );
        }
    }

    return defaultValue;
}

size_t NamedProperties::GetSize() const
{
    return m_NamedProperties.size();
}

TypePropertiesBase* NamedProperties::FindInMap( const std::string& key )
{
    const std::map<std::string, TypePropertiesBase*>::const_iterator search = m_NamedProperties.find( key );
    if( search == m_NamedProperties.cend() )
    {
        return nullptr;
    }

    return search->second;
}
