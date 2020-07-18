#include "Tags.hpp"

bool Tags::HasTag( const std::string& tag ) const
{
    for ( int tagIndex = 0; tagIndex < tags.size(); ++tagIndex )
    {
        if ( Stricmp( tag, tags.at( tagIndex ) ) == 0 )
        {
            return true;
        }
    }

    return false;
}

bool Tags::HasTags( const Strings& tagList ) const
{
    if ( tags.size() == 0 )
    {
        // Zero tags only matters if there is a non negative tag in the tagList
        for ( int tagListIndex = 0; tagListIndex < tagList.size(); ++tagListIndex )
        {
            const std::string& tagInTagList = tagList.at( tagListIndex );
            if ( tagInTagList.at( 0 ) != '!' )
            {
                return false;
            }
        }
        return true;
    }

    for ( int tagListIndex = 0; tagListIndex < tagList.size(); ++tagListIndex )
    {
        std::string tagInTagList = tagList.at( tagListIndex );
        // If the first character is '!' look if tag is NOT in list
        bool compareTo = tagInTagList.at( 0 ) == '!' ? false : true;
        if ( compareTo == false )
        {
            // Remove '!'
            tagInTagList = tagInTagList.substr( 1 );
        }

        bool foundMatchingTag = false;
        for ( int tagIndex = 0; tagIndex < tags.size(); ++tagIndex )
        {
            const std::string& tag = tags.at( tagIndex );
            if ( Stricmp( tagInTagList, tag ) == 0 )
            {
                // If found comparison to '!' tag
                if ( !compareTo )
                {
                    return false;
                }
                else
                {
                    foundMatchingTag = true;
                    break;
                }
            }
        }

        // If no matching tag was found
        if ( !foundMatchingTag )
        {
            // Return the opposite of what I want to compare to
            return !compareTo;
        }
    }

    return true;

//     for ( int tagIndex = 0; tagIndex < tags.size(); ++tagIndex )
//     {
//         const std::string& tag = tags.at( tagIndex );
//         bool foundTag = false;
//         for ( int tileTagIndex = 0; tileTagIndex < tagList.size(); ++tileTagIndex )
//         {
//             const std::string& tileTag = tagList.at( tileTagIndex );
//             if ( Stricmp( tag, tileTag ) == 0 )
//             {
//                 foundTag = true;
//                 break;
//             }
//         }
// 
//         if ( !foundTag )
//         {
//             return false;
//         }
//     }
//     return true;
}

bool Tags::Contains( const Tags& otherTags ) const
{
    return HasTags(otherTags.tags);
}

void Tags::AddTag( const std::string& tag )
{
    std::string tempTag = tag;
    Trim( tempTag );

    tags.push_back( tempTag );
}

void Tags::AddTags( const Strings& tagList )
{
    for ( int tagIndex = 0; tagIndex < tagList.size(); ++tagIndex )
    {
        AddTag( tagList.at( tagIndex ) );
    }
}

void Tags::AddTags( const Tags& otherTags )
{
    AddTags( otherTags.tags );
}

void Tags::RemoveTag( const std::string& tag )
{
    Strings::const_iterator foundTag = tags.cbegin();
    for ( ; foundTag < tags.cend(); ++foundTag )
    {
        if ( Stricmp( *foundTag, tag ) == 0 )
        {
            tags.erase( foundTag );
        }
    }
}

void Tags::RemoveTags( const Strings& tagList )
{
    for ( int tagIndex = 0; tagIndex < tagList.size(); ++tagIndex )
    {
        RemoveTag( tagList.at( tagIndex ) );
    }
}

void Tags::Clear()
{
    tags.clear();
}
