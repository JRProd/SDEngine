#pragma once

#include <vector>

// Extension of std::vector
// Use this to optimize replacing values in the vector
// Only compatible with pointers
template <typename T>
class RapidReplaceVector
{
    static_assert(std::is_pointer<T>::value, "RapidReplateVector: Expected a pointer");

public:
    std::vector<T> data;

    RapidReplaceVector() {}

    size_t RapidReplace( const T& value );
    void RapidRemoval( size_t index );

private:
    bool m_UseEarliestRelease = false;
    size_t m_EarliestRelease = 0;

};

template <typename T>
inline size_t RapidReplaceVector<T>::RapidReplace( const T& value )
{
    if ( m_UseEarliestRelease )
    {
        for ( size_t index = m_EarliestRelease; index < data.size(); ++index )
        {
            T& currentValue = data[ index ];
            if ( currentValue == nullptr )
            {
                currentValue = value;
                return index;
            }
        }
    }

    m_UseEarliestRelease = false;

    data.push_back( value );
    return data.size() - 1;
}

template <typename T>
inline void RapidReplaceVector<T>::RapidRemoval( size_t index )
{
    if ( !m_UseEarliestRelease )
    {
        m_EarliestRelease = index;
        m_UseEarliestRelease = true;
    }
    else if ( index < m_EarliestRelease )
    {
        m_EarliestRelease = index;
    }

    delete data[ index ];
    data[ index ] = nullptr;
}