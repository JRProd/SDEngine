#include "Light.hpp"

#include "Engine/Core/Rgba8.hpp"

bool Light::operator==( const Light& rhs ) const
{
    return !this->operator!=( rhs );
}

bool Light::operator!=( const Light& rhs ) const
{
    return position != rhs.position || color != rhs.color || attenuation != rhs.attenuation || specularAttenuation != rhs.specularAttenuation;
}

