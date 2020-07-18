#include "XmlUtils.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Range/IntRange.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"

bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue )
{
    const char* boolValue = element.Attribute( attributeName );
    if ( boolValue )
    {
        return ConvertFromString( boolValue, defaultValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using BOOL default value \"%s\"",
                                          defaultValue ? "TRUE" : "FALSE" ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue )
{
    const char* charValue = element.Attribute( attributeName );
    if ( charValue )
    {
        return *charValue;
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using CHAR default value \"%c\"",
                                          defaultValue ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue )
{
    const char* intValue = element.Attribute( attributeName );
    if ( intValue )
    {
        return atoi( intValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using INT default value \"%d\"",
                                          defaultValue ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue )
{
    const char* floatValue = element.Attribute( attributeName );
    if ( floatValue )
    {
        return strtof( floatValue, nullptr );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using FLOAT default value \"%f\"",
                                          defaultValue ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, std::string defaultValue )
{
    const char* stringValue = element.Attribute( attributeName );
    if ( stringValue )
    {
        return std::string( stringValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using STRING default value \"%s\"",
                                          defaultValue.c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue )
{
    const char* stringValue = element.Attribute( attributeName );
    if ( stringValue )
    {
        return std::string( stringValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using STRING default value \"%s\"",
                                          defaultValue ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const Rgba8 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue )
{
    const char* rgbaValue = element.Attribute( (attributeName) );
    if ( rgbaValue )
    {
        return Rgba8::CreateFromString( rgbaValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using RGBA8 default value \"%s\"",
                                          ConvertToString(defaultValue).c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const AABB2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue )
{
    const char* aabb2Value = element.Attribute( (attributeName) );
    if ( aabb2Value )
    {
        return AABB2::CreateFromString( aabb2Value );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using AABB2 default value \"%s\"",
                                          ConvertToString(defaultValue).c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue )
{
    const char* intVec2Value = element.Attribute( (attributeName) );
    if ( intVec2Value )
    {
        return IntVec2::CreateFromString( intVec2Value );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using INT_VEC2 default value \"%s\"",
                                          defaultValue.ToString().c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue )
{
    const char* vec2Value = element.Attribute( (attributeName) );
    if ( vec2Value )
    {
        return Vec2::CreateFromString( vec2Value );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using VEC2 default value \"%s\"",
                                          ConvertToString(defaultValue).c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue )
{
    const char* intRangeValue = element.Attribute( (attributeName) );
    if ( intRangeValue )
    {
        return IntRange::CreateFromString( intRangeValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using INT_RANGE default value \"%s\"",
                                          defaultValue.ToString().c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}

const FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue )
{
    const char* floatRangeValue = element.Attribute( (attributeName) );
    if ( floatRangeValue )
    {
        return FloatRange::CreateFromString( floatRangeValue );
    }

#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_WARNING, Stringf( "XmlUtils::ParseXmlAttribute - Using FLOAT_RANGE default value \"%s\"",
                                          defaultValue.ToString().c_str() ) );
#endif // !defined( ENGINE_DISABLE_CONSOLE )
    return defaultValue;
}
