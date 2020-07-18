#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Math/Primatives/Vec3.hpp"

#include <map>
#include <string>
#include <vector>

namespace tinyxml2 {
    class XMLElement;
}
typedef tinyxml2::XMLElement XmlElement;

typedef std::map<std::string, std::string> StringPairMap;

class NamedStrings
{
public:
    ~NamedStrings() = default;
    NamedStrings() = default;

    // Static Methods
    static NamedStrings MakeFromXmlElement( const XmlElement& element );
    static NamedStrings MakeFromString( const std::string& keyValuePairs );

    // Accessors (const)
    bool                GetValue( const std::string& keyName, bool defaultValue ) const;
    char                GetValue( const std::string& keyName, char defaultValue ) const;
    unsigned char       GetValue( const std::string& keyName, unsigned char defaultValue ) const;
    int                 GetValue( const std::string& keyName, int defaultValue ) const;
    unsigned int        GetValue( const std::string& keyName, unsigned int defaultValue ) const;
    float               GetValue( const std::string& keyName, float defaultValue ) const;
    std::string   GetValue( const std::string& keyName, const char* defaultValue ) const;
    std::string   GetValue( const std::string& keyName, const std::string& defaultValue ) const;
    Rgba8         GetValue( const std::string& keyName, const Rgba8& defaultValue ) const;
    IntVec2       GetValue( const std::string& keyName, const IntVec2& defaultValue ) const;
    Vec2          GetValue( const std::string& keyName, const Vec2& defaultValue ) const;
    Vec3          GetValue( const std::string& keyName, const Vec3& defaultValue ) const;

    bool                GetValue( const std::vector<std::string>& keyNames, bool defaultValue ) const;
    char                GetValue( const std::vector<std::string>& keyNames, char defaultValue ) const;
    unsigned char       GetValue( const std::vector<std::string>& keyNames, unsigned char defaultValue ) const;
    int                 GetValue( const std::vector<std::string>& keyNames, int defaultValue ) const;
    unsigned int        GetValue( const std::vector<std::string>& keyNames, unsigned int defaultValue ) const;
    float               GetValue( const std::vector<std::string>& keyNames, float defaultValue ) const;
    std::string   GetValue( const std::vector<std::string>& keyNames, const char* defaultValue ) const;
    std::string   GetValue( const std::vector<std::string>& keyNames, const std::string& defaultValue ) const;
    Rgba8         GetValue( const std::vector<std::string>& keyNames, const Rgba8& defaultValue ) const;
    IntVec2       GetValue( const std::vector<std::string>& keyNames, const IntVec2& defaultValue ) const;
    Vec2          GetValue( const std::vector<std::string>& keyNames, const Vec2& defaultValue ) const;
    Vec3          GetValue( const std::vector<std::string>& keyNames, const Vec3& defaultValue ) const;

    size_t GetSize() const;
    void RemoveValue( const char* string );
    void RemoveValue( const std::string& string );

    // Mutators (non-const)
    void                SetValue( const std::string& key, const std::string& value );
private:
    StringPairMap m_KeyValuePairs;
};