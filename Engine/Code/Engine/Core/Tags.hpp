#pragma once

#include <string>
#include <vector>

#include "Engine/Core/Utils/StringUtils.hpp"

struct Tags
{
    Strings tags;

public:
    bool HasTag( const std::string& tag ) const;
    bool HasTags( const Strings& tagList ) const;
    bool Contains( const Tags& otherTags ) const;

    void AddTag( const std::string& tag );
    void AddTags( const Strings& tagList );
    void AddTags( const Tags& otherTags );
    void RemoveTag( const std::string& tag );
    void RemoveTags( const Strings& tagList );

    void Clear();
};