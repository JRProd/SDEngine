#pragma once

#include "Engine/Core/Utils/StringUtils.hpp"

#include "ThirdParty/tinyxml2/tinyxml2.h"

#include <string>
#include <vector>

struct AABB2;
struct IntVec2;
struct Rgba8;
struct Vec2;

struct IntRange;
struct FloatRange;

// Using TinyXML2 for XML parsing. Refers to tinyxml2::XMLDocument
typedef tinyxml2::XMLDocument XmlDocument;
// Using TinyXML2 for XML parsing. Refers to tinyxml2::XMLElement
typedef tinyxml2::XMLElement XmlElement;
// Using TinyXML2 for XML parsing. Refers to tinyxml2::XMLAttribute
typedef tinyxml2::XMLAttribute XmlAttribute;

//-----------------------------------------------------------------------------
// Primitive types
bool ParseXmlAttribute( const XmlElement& element,
                        const char* attributeName,
                        bool defaultValue );
char ParseXmlAttribute( const XmlElement& element,
                        const char* attributeName,
                        char defaultValue );
int ParseXmlAttribute( const XmlElement& element,
                       const char* attributeName,
                       int defaultValue );
float ParseXmlAttribute( const XmlElement& element,
                         const char* attributeName,
                         float defaultValue );
template <typename ObjectType>
std::vector<ObjectType> ParseXmlAttributeToList( const XmlElement& element, const char& attributeName, char delimiter );

//-----------------------------------------------------------------------------
// C++ types
const std::string ParseXmlAttribute( const XmlElement& element,
                                     const char* attributeName,
                                     std::string defaultValue = "" );
const std::string ParseXmlAttribute( const XmlElement& element,
                                     const char* attributeName,
                                     const char* defaultValue );

//-----------------------------------------------------------------------------
// Engine Types
const AABB2 ParseXmlAttribute( const XmlElement& element,
                               const char* attributeName,
                               const AABB2& defaultValue );
const IntVec2 ParseXmlAttribute( const XmlElement& element,
                                 const char* attributeName,
                                 const IntVec2& defaultValue );
const Rgba8 ParseXmlAttribute( const XmlElement& element,
                               const char* attributeName,
                               const Rgba8& defaultValue );
const Vec2 ParseXmlAttribute( const XmlElement& element,
                              const char* attributeName,
                              const Vec2& defaultValue );

const IntRange ParseXmlAttribute( const XmlElement& element,
                                  const char* attributeName,
                                  const IntRange& defaultValue );
const FloatRange ParseXmlAttribute( const XmlElement& element,
                                    const char* attributeName,
                                    const FloatRange& defaultValue );

template<typename ObjectType>
std::vector<ObjectType> ParseXmlAttributeToList( const XmlElement& element, const char* attributeName, char delimiter )
{
    const std::string elementValue = ParseXmlAttribute( element, attributeName );
    std::vector<std::string> splitValue = SplitStringOnDelimiter( elementValue, delimiter );
    if( splitValue.empty() )
    {
        return std::vector<ObjectType>();
    }

    ObjectType type = ObjectType();
    std::vector<ObjectType> objects;
    for( std::string& string : splitValue )
    {
        objects.push_back( ConvertFromString( string, type ) );
    }

    return objects;
}
