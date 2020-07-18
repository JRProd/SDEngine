#pragma once

#include "Engine/Event/EventSystem.hpp"
#include "Engine/Core/Utils/TypePropertyUtils.hpp"

struct Rgba8;

struct Argument
{
    std::string tag;
    TypePropertiesBase* typeProperties = nullptr;
    bool optional = true;
    bool optionalTag = true;

    Argument( const std::string& commandTag, bool isOptional = true, bool isOptionalTag = true );
    Argument( const Argument& createFrom );

    virtual ~Argument();
};

template <typename Type>
struct TypedArgument: Argument
{
    TypedArgument( const std::string& commandTag, bool isOptional = true, bool isOptionalTag = true );
    virtual ~TypedArgument();
};

struct Command
{
    EventName commandName;
    std::vector<Argument*> arguments;
    std::string description;

public:
    std::string ToMarkdownString( const Rgba8& commandColor, const Rgba8& optionalColor,
                                  const Rgba8& requiredColor, const Rgba8& descriptionColor );

    void FormatNamedString( OUT_PARAM NamedStrings& namedStrings ) const;
    std::map<std::string, TypePropertiesBase*> GetArgumentTypes() const;

    Command() = default;
    ~Command();
    Command( const Command& createFrom );

private:
    std::string m_MarkedString;
};

template<typename Type>
TypedArgument<Type>::TypedArgument( const std::string& commandTag, bool isOptional, bool isOptionalTag )
    : Argument( commandTag, isOptional, isOptionalTag )
{
    typeProperties = new TypeProperties<Type>();
}

template<typename Type>
inline TypedArgument<Type>::~TypedArgument()
{
    if ( typeProperties != nullptr )
    {
        delete typeProperties;
        typeProperties = nullptr;
    }
};