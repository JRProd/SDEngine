#include "Shader.hpp"

#include "Engine/Renderer/RenderContext.hpp"

static BlendMode GetBlendModeXml( const XmlElement& element, const BlendMode defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "blendMode" );
    if ( string == "alpha" ) { return BlendMode::ALPHA; }
    if ( string == "additive" ) { return BlendMode::ADDITIVE; }
    if ( string == "solid" ) { return  BlendMode::SOLID; }

    return defaultValue;
}

static DepthCompare GetDepthCompareFromXml( const XmlElement& element, const DepthCompare defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "depthTest" );
    if ( string == "never" ) { return DepthCompare::NEVER; }
    if ( string == "always" ) { return DepthCompare::ALWAYS; }
    if ( string == "equal" ) { return DepthCompare::EQUAL; }
    if ( string == "greater" ) { return DepthCompare::GREATER; }
    if ( string == "greaterEqual" ) { return DepthCompare::GREATER_EQUAL; }
    if ( string == "less" ) { return DepthCompare::LESS; }
    if ( string == "lessEqual" ) { return DepthCompare::LESS_EQUAL; }

    return defaultValue;
}

static CullMode GetCullModeFromXml( const XmlElement& element, const CullMode defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "cullMode" );
    if ( string == "none" ) { return CullMode::NONE; }
    if ( string == "front" ) { return CullMode::FRONT; }
    if ( string == "back" ) { return CullMode::BACK; }

    return defaultValue;
}

static FillMode GetFillModeFromXml( const XmlElement& element, const FillMode defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "fillMode" );
    if ( string == "line" ) { return FillMode::LINE; }
    if ( string == "triangle" ) { return FillMode::TRIANGLE; }

    return defaultValue;
}

static WindOrder GetWindOrderFromXml( const XmlElement& element, const WindOrder defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "windOrder" );
    if ( string == "counterClockwise" ) { return WindOrder::COUNTER_CLOCKWISE; }
    if ( string == "clockwise" ) { return WindOrder::CLOCKWISE; }

    return defaultValue;
}

Shader::Shader( ShaderProgram* shaderProgram )
    : program( shaderProgram )
{
}

Shader* Shader::PopulateFromXmlElement( RenderContext* ctx, const XmlElement& element )
{
    const std::string programFile = ParseXmlAttribute( element,"program", 
                                                 "Data/Shaders/default/default.hlsl" );
    ShaderProgram* program = ctx->CreateOrGetShaderProgramFromFile( programFile );
    Shader* createdShader = new Shader( program );
    createdShader->blendMode = GetBlendModeXml( element, createdShader->blendMode );

    const XmlElement* depthTest = element.FirstChildElement( "DepthState" );
    if( depthTest != nullptr )
    {
        createdShader->depthCompare = GetDepthCompareFromXml( *depthTest, createdShader->depthCompare );
        createdShader->writeDepth = ParseXmlAttribute( *depthTest, "depthWrite", createdShader->writeDepth );
    }

    const XmlElement* rasterizer = element.FirstChildElement( "Rasterizer" );
    if( rasterizer != nullptr )
    {
        createdShader->cullMode = GetCullModeFromXml( *rasterizer, createdShader->cullMode );
        createdShader->fillMode = GetFillModeFromXml( *rasterizer, createdShader->fillMode );
        createdShader->windOrder = GetWindOrderFromXml( *rasterizer, createdShader->windOrder );
    }

    return createdShader;
}

bool Shader::operator==( const Shader& rhs ) const
{
    return !operator!=( rhs );
}

bool Shader::operator!=( const Shader& rhs ) const
{
    return program != rhs.program || blendMode != rhs.blendMode || depthCompare != rhs.depthCompare ||
        writeDepth != rhs.writeDepth || windOrder != rhs.windOrder || cullMode != rhs.cullMode || 
        fillMode != rhs.fillMode;
}

Shader& Shader::operator=( const Shader& rhs )
{
    if ( this == &rhs ) { return *this; }   // Self assignment

    program = rhs.program;
    blendMode = rhs.blendMode;
    depthCompare = rhs.depthCompare;
    writeDepth = rhs.writeDepth;
    windOrder = rhs.windOrder;
    cullMode = rhs.cullMode;
    fillMode = rhs.fillMode;

    return *this;
}
