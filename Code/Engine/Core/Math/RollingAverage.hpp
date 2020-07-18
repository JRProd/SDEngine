#pragma once

template <typename T>
class RollingAverage
{
public:
    RollingAverage( int size );
    ~RollingAverage();

    T GetAverage();
    void Add( T value );

private:
    T* m_Values = nullptr;
    int m_Size;
    int m_NextIndex = 0;

    bool m_IsFilled = false;
};

template<class T>
inline RollingAverage<T>::RollingAverage( int size )
    : m_Size( size )
{
    m_Values = new T[ size ];
}

template<class T>
inline RollingAverage<T>::~RollingAverage()
{
    if ( m_Values != nullptr )
    {
        delete[] m_Values;
    }
}

template<class T>
inline T RollingAverage<T>::GetAverage()
{
    if ( !m_IsFilled && m_NextIndex == 0 )
    {
        return T();
    }

    int size = m_IsFilled ? m_Size : m_NextIndex;

    T value = m_Values[ 0 ];
    for ( int index = 1; index < size; ++index )
    {
        value += m_Values[ index ];
    }

    value /= static_cast<float>(m_Size);
    return value;
}

template<class T>
inline void RollingAverage<T>::Add( T value )
{
    m_Values[ m_NextIndex ] = value;
    m_NextIndex++;
    if ( m_NextIndex >= m_Size )
    {
        m_NextIndex = 0;
        m_IsFilled = true;
    }
}
