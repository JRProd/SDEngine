#pragma once
#include "Engine/Core/Utils/XmlUtils.hpp"

class RenderContext;
class ShaderProgram;

enum class BlendMode
{
    ALPHA,
    SOLID,
    ADDITIVE,
};

enum class DepthCompare
{
    NEVER,
    ALWAYS,
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
};

enum class CullMode
{
    NONE,
    FRONT,
    BACK,
};

enum class FillMode
{
    TRIANGLE,
    LINE,
};

enum class WindOrder
{
    CLOCKWISE,
    COUNTER_CLOCKWISE,
};

struct Shader
{
    const ShaderProgram* program = nullptr;

    BlendMode blendMode = BlendMode::ALPHA;
    DepthCompare depthCompare = DepthCompare::LESS_EQUAL;
    bool writeDepth = true;
    WindOrder windOrder = WindOrder::COUNTER_CLOCKWISE;
    CullMode cullMode = CullMode::BACK;
    FillMode fillMode = FillMode::TRIANGLE;

    explicit Shader( ShaderProgram* shaderProgram );

    static Shader* PopulateFromXmlElement( RenderContext* ctx, const XmlElement& element );

    bool operator==( const Shader& rhs ) const;
    bool operator!=( const Shader& rhs ) const;
    Shader& operator=( const Shader& rhs );
};
