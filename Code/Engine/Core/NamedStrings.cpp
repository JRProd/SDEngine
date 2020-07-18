#include "NamedStrings.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"

EXTERNED NamedStrings g_GameConfigBlackboard = NamedStrings();

NamedStrings NamedStrings::MakeFromXmlElement( const XmlElement& element )
{
    NamedStrings pairsFromElement;
    const XmlAttribute* attribute = element.FirstAttribute();
    while ( attribute != nullptr )
    {
        pairsFromElement.SetValue( attribute->Name(),
                                   attribute->Value() );
        attribute = attribute->Next();
    }

    return pairsFromElement;
}

static void AddArgOrTokenHelper( OUT_PARAM NamedStrings& namedStrings, const std::string& tag,
                                 const std::string& arg, OUT_PARAM bool& namedToken,
                                 OUT_PARAM size_t& currentArgNumber )
{
    if ( namedToken )
    {
        namedStrings.SetValue( tag, arg );
        namedToken = false;
    }
    else
    {
        namedStrings.SetValue( "Arg" + std::to_string( currentArgNumber ), arg );
        currentArgNumber++;
    }
}

NamedStrings NamedStrings::MakeFromString( const std::string& keyValuePairs )
{
    NamedStrings namedStrings;

    size_t lastSplitIndex = 0;
    bool inQuotes = false;
    bool quoteIsLast = false;
    bool namedToken = false;

    std::string tag;
    std::string arg;

    size_t currentArgNumber = 1;
    for ( size_t splitIndex = 0; splitIndex < keyValuePairs.size(); ++splitIndex )
    {
        const char& currentCharacter = keyValuePairs.at( splitIndex );

        // Remove whitespace
        if ( currentCharacter == ' ' && lastSplitIndex == splitIndex )
        {
            if ( !inQuotes )
            {
                // If the previous character was '"'
                if ( lastSplitIndex > 0 && keyValuePairs.at( lastSplitIndex - 1 ) == '"' )
                {
                    AddArgOrTokenHelper( namedStrings, tag, arg, namedToken, currentArgNumber );
                }

                lastSplitIndex = splitIndex + 1;
                continue;
            }
        }

        // Checking a whitespace sequence
        if ( currentCharacter == ' ' )
        {
            if ( inQuotes ) { continue; }

            const int count = static_cast<int>(splitIndex - lastSplitIndex);
            arg = keyValuePairs.substr( lastSplitIndex, count );

            lastSplitIndex = splitIndex + 1;

            AddArgOrTokenHelper( namedStrings, tag, arg, namedToken, currentArgNumber );
        }

        // Checking a quote sequence
        if ( currentCharacter == '"' )
        {
            if ( !inQuotes )
            {
                inQuotes = true;
                continue;
            }
            if ( keyValuePairs.at( splitIndex - 1 ) != '\\' )
            {
                // Ignore the '"'
                lastSplitIndex++;
                const int count = static_cast<int>(splitIndex - lastSplitIndex);
                arg = keyValuePairs.substr( lastSplitIndex, count );

                if ( splitIndex + 1 < keyValuePairs.size() )
                {
                    lastSplitIndex = splitIndex + 1;
                }
                else
                {
                    quoteIsLast = true;
                }
                inQuotes = false;
            }
        }

        // Checking a tagged sequence
        if ( currentCharacter == '=' )
        {
            if ( inQuotes ) { continue; }

            if ( !namedToken )
            {
                namedToken = true;
                const int count = static_cast<int>(splitIndex - lastSplitIndex);
                arg = keyValuePairs.substr( lastSplitIndex, count );

                lastSplitIndex = splitIndex;

                tag = arg;

                // Ignore the '='
                lastSplitIndex++;
            }
        }
    }

    if ( inQuotes ) { ERROR_AND_DIE( "MakeFromString: Missing end quotes" ); }

    // Get the last sequence
    if ( lastSplitIndex < keyValuePairs.size() )
    {
        if ( !quoteIsLast )
        {
            arg = keyValuePairs.substr( lastSplitIndex );
        }

        AddArgOrTokenHelper( namedStrings, tag, arg, namedToken, currentArgNumber );
    }

    return namedStrings;
}

bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return ConvertFromString( foundIndex->second, defaultValue );
    }
    return defaultValue;
}

char NamedStrings::GetValue( const std::string& keyName, const char defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return static_cast<char>(foundIndex->second.at( 0 ));
    }
    return defaultValue;
}

unsigned char NamedStrings::GetValue( const std::string& keyName, const unsigned char defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return static_cast<unsigned char>(foundIndex->second.at( 0 ));
    }
    return defaultValue;
}

int NamedStrings::GetValue( const std::string& keyName, const int defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return ConvertFromString( foundIndex->second, defaultValue );
    }
    return defaultValue;
}

unsigned int NamedStrings::GetValue( const std::string& keyName, const unsigned int defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return ConvertFromString( foundIndex->second, defaultValue );
    }
    return defaultValue;
}

float NamedStrings::GetValue( const std::string& keyName, const float defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return ConvertFromString( foundIndex->second, defaultValue );
    }
    return defaultValue;
}

std::string NamedStrings::GetValue( const std::string& keyName,
                                    const char* defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return foundIndex->second;
    }
    return defaultValue;
}

std::string NamedStrings::GetValue( const std::string& keyName,
                                    const std::string& defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return foundIndex->second;
    }
    return defaultValue;
}

Rgba8 NamedStrings::GetValue( const std::string& keyName, const Rgba8& defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return Rgba8::CreateFromString( foundIndex->second.c_str() );
    }
    return defaultValue;
}

IntVec2 NamedStrings::GetValue( const std::string& keyName,
                                const IntVec2& defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return IntVec2::CreateFromString( foundIndex->second.c_str() );
    }
    return defaultValue;
}

Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return Vec2::CreateFromString( foundIndex->second.c_str() );
    }
    return defaultValue;
}

Vec3 NamedStrings::GetValue( const std::string& keyName, const Vec3& defaultValue ) const
{
    StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
    if ( foundIndex != m_KeyValuePairs.cend() )
    {
        return Vec3::CreateFromString( foundIndex->second.c_str() );
    }
    return defaultValue;
}

bool NamedStrings::GetValue( const std::vector<std::string>& keyNames, bool defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return ConvertFromString( foundIndex->second, defaultValue );
        }
    }
    return defaultValue;
}

char NamedStrings::GetValue( const std::vector<std::string>& keyNames, char defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return static_cast<char>(foundIndex->second.at( 0 ));
        }
    }
    return defaultValue;
}

unsigned char NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                                      unsigned char defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return static_cast<unsigned char>(foundIndex->second.at( 0 ));
        }
    }
    return defaultValue;
}

int NamedStrings::GetValue( const std::vector<std::string>& keyNames, int defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return ConvertFromString( foundIndex->second, defaultValue );
        }
    }
    return defaultValue;
}

unsigned int NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                                     unsigned int defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return ConvertFromString( foundIndex->second, defaultValue );
        }
    }
    return defaultValue;
}

float NamedStrings::GetValue( const std::vector<std::string>& keyNames, float defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return ConvertFromString( foundIndex->second, defaultValue );
        }
    }
    return defaultValue;
}

std::string NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                                    const char* defaultValue ) const
{
    return GetValue( keyNames, std::string(defaultValue) );
}

std::string NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                                    const std::string& defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return foundIndex->second;
        }
    }

    return defaultValue;
}

Rgba8 NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                              const Rgba8& defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return Rgba8::CreateFromString( foundIndex->second.c_str() );
        }
    }
    return defaultValue;
}

IntVec2 NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                                const IntVec2& defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return IntVec2::CreateFromString( foundIndex->second.c_str() );
        }
    }
    return defaultValue;
}

Vec2 NamedStrings::GetValue( const std::vector<std::string>& keyNames,
                             const Vec2& defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return Vec2::CreateFromString( foundIndex->second.c_str() );
        }
    }
    return defaultValue;
}

Vec3 NamedStrings::GetValue( const std::vector<std::string>& keyNames, const Vec3& defaultValue ) const
{
    for ( const std::string& keyName : keyNames )
    {
        StringPairMap::const_iterator foundIndex = m_KeyValuePairs.find( keyName );
        if ( foundIndex != m_KeyValuePairs.cend() )
        {
            return Vec3::CreateFromString( foundIndex->second.c_str() );
        }
    }
    return defaultValue;
}

size_t NamedStrings::GetSize() const
{
    return m_KeyValuePairs.size();
}

void NamedStrings::RemoveValue( const char* string )
{
    RemoveValue( std::string( string ) );
}

void NamedStrings::RemoveValue( const std::string& string )
{
    m_KeyValuePairs.erase( string );
}

void NamedStrings::SetValue( const std::string& key, const std::string& value )
{
#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_VERBOSE, Stringf( "NamedStrings: Adding [%s, %s]",
                                          key.c_str(),
                                          value.c_str() ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE
    m_KeyValuePairs[ key ] = value;
}
