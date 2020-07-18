#pragma once

#include "Engine/Core/Utils/StringUtils.hpp"

enum class BufferFormatType
{
    VEC2,
    VEC3,
    R8G8B8A8_UNORM,

    FINAL,
};

struct BufferAttribute
{
public:
    std::string name;
    unsigned int location;
    BufferFormatType type;
    unsigned int offset;

    BufferAttribute( const char* semanticName, unsigned int loc, BufferFormatType attributeType, unsigned int offset );
    BufferAttribute();

    bool operator==( const BufferAttribute& rhs ) const;
};