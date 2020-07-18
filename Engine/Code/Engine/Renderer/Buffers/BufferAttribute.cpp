#include "BufferAttribute.hpp"

#include "Engine/Core/Utils/StringUtils.hpp"

BufferAttribute::BufferAttribute( const char* semanticName,
                                        unsigned int loc,
                                        BufferFormatType attributeType,
                                        unsigned int attributeOffset )
    : name( semanticName )
    , location( loc )
    , type( attributeType )
    , offset( attributeOffset )
{
}

BufferAttribute::BufferAttribute()
    : name( "FINAL_ATTRIBUTE" )
    , location( 0 )
    , type( BufferFormatType::FINAL )
    , offset( 0 )
{
}

bool BufferAttribute::operator==( const BufferAttribute& rhs ) const
{
    return name.compare( rhs.name ) == 0 && location == rhs.location && type == rhs.type && offset == rhs.offset;
}
