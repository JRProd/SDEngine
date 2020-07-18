#include "IntRange.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"

//-----------------------------------------------------------------------------
IntRange::IntRange( int minAndMax )
    : minimum( minAndMax )
    , maximum( minAndMax )
{
}

//-----------------------------------------------------------------------------
IntRange::IntRange( int min, int max )
    : minimum( min )
    , maximum( max )
{
}

//-----------------------------------------------------------------------------
STATIC IntRange IntRange::CreateFromString( const char* data )
{
    Strings splitIntRange = SplitStringOnDelimiter( data, '~' );

    IntRange value;
    if ( splitIntRange.size() == 1 )
    {
        int minMax = stoi( splitIntRange.at( 0 ) );
        value.minimum = minMax;
        value.maximum = minMax;
    }
    else if ( splitIntRange.size() == 2 )
    {
        value.minimum = stoi( splitIntRange.at( 0 ) );
        value.maximum = stoi( splitIntRange.at( 1 ) );
    }
    else
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "IntRange::CreateFromString - Recieved invalid sequence \"%s\"", data ) );
    #endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

//-----------------------------------------------------------------------------
bool IntRange::IsInRange( int testPoint ) const
{
    if ( testPoint < minimum ) { return false; }
    if ( testPoint > maximum ) { return false; }
    return true;
}

//-----------------------------------------------------------------------------
bool IntRange::DoesOverlap( const IntRange& otherRange ) const
{
    if ( minimum >= otherRange.minimum && minimum <= otherRange.maximum )
    {
        return true;
    }
    if ( maximum >= otherRange.minimum && maximum <= otherRange.maximum )
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
int IntRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
    return rng.IntInRange(minimum, maximum);
}

//-----------------------------------------------------------------------------
void IntRange::Set( int min, int max )
{
    minimum = min;
    maximum = max;
}

//-----------------------------------------------------------------------------
const std::string IntRange::ToString() const
{
    return Stringf("%d~%d", minimum, maximum);
}
