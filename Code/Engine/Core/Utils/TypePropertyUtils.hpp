#pragma once

#include "StringUtils.hpp"

#include <string>

template <typename Value>
class TypeProperties;

class TypePropertiesBase
{
public:
    TypePropertiesBase() = default;
    virtual ~TypePropertiesBase() = default;

    virtual TypePropertiesBase* CreateCopy() const = 0;

    virtual std::string ToString() const = 0;
    virtual std::string TypeName() const = 0;
    virtual const void* GetUniqueId() const = 0;

    virtual void SetFromString( const std::string& string ) = 0;

    template<typename Value>
    bool Is() const
    {
        return GetUniqueId() == TypeProperties<Value>::UNIQUE_ID;
    }
};

template <typename  Value>
class TypeProperties: public TypePropertiesBase
{
    friend class NamedProperties;

private:
    static const unsigned char m_UniqueByte = 0;

public:
    static constexpr void const* const UNIQUE_ID = &m_UniqueByte;

    TypeProperties() = default;

    TypePropertiesBase* CreateCopy() const override;

    std::string ToString() const override;
    std::string TypeName() const override;
    const void* GetUniqueId() const override;

    void SetFromString( const std::string& string ) override;

private:
    Value m_Value;
};

template<typename Value>
TypePropertiesBase* TypeProperties<Value>::CreateCopy() const
{
    TypeProperties<Value>* copy = new TypeProperties<Value>();
    copy->m_Value = m_Value;
    return copy;
}

template<typename Value>
std::string TypeProperties<Value>::ToString() const
{
    return ConvertToString( m_Value );
}

template<typename Value>
std::string TypeProperties<Value>::TypeName() const
{
    return GetTypeName( m_Value );
}

template<typename Value>
const void* TypeProperties<Value>::GetUniqueId() const
{
    return UNIQUE_ID;
}

template<typename Value>
void TypeProperties<Value>::SetFromString( const std::string& string )
{
    m_Value = ConvertFromString( string, m_Value );
}
