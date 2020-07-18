#include "FloatRange.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"

#include <limits>

//-----------------------------------------------------------------------------
FloatRange::FloatRange( float minAndMax )
    : minimum(minAndMax)
    , maximum(minAndMax)
{
}

//-----------------------------------------------------------------------------
FloatRange::FloatRange( float min, float max )
    : minimum(min)
    , maximum(max)
{
    Fix();
}

//-----------------------------------------------------------------------------
STATIC FloatRange FloatRange::CreateFromString( const char* data )
{
    Strings splitFloatRange = SplitStringOnDelimiter( data, '~' );

    FloatRange value;
    if ( splitFloatRange.size() == 1 )
    {
        float minMax = stof( splitFloatRange.at( 0 ) );
        value.minimum = minMax;
        value.maximum = minMax;
    }
    else if ( splitFloatRange.size() == 2 )
    {
        value.minimum = stof( splitFloatRange.at( 0 ) );
        value.maximum = stof( splitFloatRange.at( 1 ) );
    }
    else
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "FloatRange::CreateFromString - Recieved invalid sequence \"%s\"", data ) );
    #endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

FloatRange FloatRange::CreateFromPointsOnAxis( int numPoints, const Vec2* points, const Vec2& relativePoint, const Vec2& normalizedAxis, float radius )
{
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();
    for ( int currentIndex = 0; currentIndex < numPoints; ++currentIndex )
    {
        Vec2 localDisplacement = points[ currentIndex ] - relativePoint;
        float pointOnAxis = localDisplacement.GetProjectedLength( normalizedAxis );

        if ( pointOnAxis <= min )
        {
            min = pointOnAxis;
        }
        if ( pointOnAxis >= max )
        {
            max = pointOnAxis;
        }
    }

    return FloatRange(min - radius, max + radius);
}

//-----------------------------------------------------------------------------
bool FloatRange::IsInRange( float testPoint ) const
{
    if ( testPoint < minimum ) { return false; }
    if ( testPoint > maximum ) { return false; }
    return true;
}

//-----------------------------------------------------------------------------
bool FloatRange::DoesOverlap( const FloatRange& otherRange ) const
{

    if ( minimum >= otherRange.minimum && minimum <= otherRange.maximum )
    {
        return true;
    }
    if ( maximum >= otherRange.minimum && maximum <= otherRange.maximum )
    {
        return true;
    }
    // Check if other is completely inside range
    if ( otherRange.minimum >= minimum && otherRange.minimum <= maximum )
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
float FloatRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
    return rng.FloatInRange(minimum, maximum);
}

//-----------------------------------------------------------------------------
void FloatRange::Set( float min, float max )
{
    minimum = min;
    maximum = max;
}

void FloatRange::ClipTo( const FloatRange& otherRange )
{
    minimum = Maxf( otherRange.minimum, minimum );
    maximum = Minf( otherRange.maximum, maximum );
}

//-----------------------------------------------------------------------------
const std::string FloatRange::ToString() const
{
    return Stringf("%f~%f", minimum, maximum);
}

void FloatRange::Fix()
{
    if (maximum < minimum )
    {
        const float temp = maximum;
        maximum = minimum;
        minimum = temp;
    }
}
