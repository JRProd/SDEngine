#include "Command.hpp"

Argument::Argument( const std::string& commandTag, const bool isOptional, const bool isOptionalTag )
    : tag( commandTag )
      , optional( isOptional )
      , optionalTag( isOptionalTag )
{
    typeProperties = new TypeProperties<std::string>();
}

Argument::Argument( const Argument& createFrom )
{
    tag = createFrom.tag;
    optional = createFrom.optional;
    optionalTag = createFrom.optionalTag;

    typeProperties = createFrom.typeProperties->CreateCopy();
}

Argument::~Argument()
{
    if ( typeProperties != nullptr )
    {
        delete typeProperties;
        typeProperties = nullptr;
    }
}

std::string Command::ToMarkdownString( const Rgba8& commandColor, const Rgba8& optionalColor,
                                       const Rgba8& requiredColor, const Rgba8& descriptionColor )
{
    if ( !m_MarkedString.empty() )
    {
        return m_MarkedString;
    }

    m_MarkedString = "[[c";
    m_MarkedString += ConvertToString(commandColor) + "]" + commandName + "]";

    if ( !arguments.empty() )
    {
        m_MarkedString += ": ";
    }
    for ( Argument* a : arguments )
    {
        std::string optionalTag = a->optionalTag ? "?" : "";
        std::string markSequence = "[[c" + (a->optional
                                                ? ConvertToString( optionalColor)
                                                : ConvertToString( requiredColor)) + "]";

        m_MarkedString += markSequence + a->tag + optionalTag + "=" + a->typeProperties->TypeName() + "] ";
    }

    if ( !description.empty() )
    {
        m_MarkedString += "- [[c" + ConvertToString( descriptionColor ) + "]" + description + "]";
    }
    return m_MarkedString;
}

void Command::FormatNamedString( OUT_PARAM NamedStrings& namedStrings ) const
{
    size_t unnamedArg = 1;
    for( const Argument* argument : arguments)
    {
        if( argument->optionalTag )
        {
            const std::string argName = "Arg" + std::to_string( unnamedArg );
            const std::string argValue = namedStrings.GetValue( argName, "" );

            if( !argValue.empty() )
            {
                namedStrings.SetValue( argument->tag, argValue );
                namedStrings.RemoveValue( argName );

                unnamedArg++;
            }
        }
    }
}

std::map<std::string, TypePropertiesBase*> Command::GetArgumentTypes() const
{
    std::map<std::string, TypePropertiesBase*> typedArguments;

    size_t unnamedArg = 1;
    for( const Argument* argument : arguments )
    {
        std::string argTag = argument->tag;
        if( argTag.empty() )
        {
            argTag = "Arg" + std::to_string( unnamedArg );
            unnamedArg += 1;
        }

        typedArguments[ argTag ] = argument->typeProperties;
    }

    return typedArguments;
}

Command::~Command()
{
    for( Argument*& argument : arguments )
    {
        if( argument != nullptr )
        {
            delete argument;
            argument = nullptr;
        }
    }
}

Command::Command( const Command& createFrom )
{
    commandName = createFrom.commandName;
    description = createFrom.description;

    for( Argument* copyArg : createFrom.arguments )
    {
        arguments.push_back( new Argument( *copyArg ) );
    }
}


