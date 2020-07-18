#include "AABB2.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"

#include <algorithm>

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
    : mins( Vec2( minX, minY ) )
    , maxs( Vec2( maxX, maxY ) )
{
}

AABB2::AABB2( const Vec2& min, const Vec2& max )
    : mins( Vec2( min ) )
    , maxs( Vec2( max ) )
{
}

AABB2::AABB2( const IntVec2& min, const IntVec2& max )
    : mins( Vec2( min ) )
    , maxs( Vec2( max ) )
{
}

STATIC const AABB2 AABB2::MakeFromAspect( float aspectRatio )
{
    AABB2 aspect = UNIT_AROUND_ZERO;
    const Vec2 newDimens = Vec2( 1.f * aspectRatio, 1.f );
    aspect.SetDimensions( newDimens );
    return aspect;
}

STATIC AABB2 const AABB2::MakeFromUnitBoxAround( const Vec2& bottomLeft )
{
    return AABB2( 0.f, 0.f, 1.f, 1.f ).GetTranslation( bottomLeft );
}

STATIC AABB2 const AABB2::MakeFromCenterBoxAround( const Vec2& center )
{
    AABB2 centerBox = AABB2( -.5f, -.5f, .5f, .5f );
    centerBox.SetCenter( center );
    return centerBox;
}

const AABB2 AABB2::CreateFromString( const char* data )
{
    Strings splitAABB2 = SplitStringOnDelimiter( data, ',' );

    AABB2 value;
    if ( splitAABB2.size() == 4 ||
         splitAABB2.at( 0 ).front() == '(' ||
         splitAABB2.at( 1 ).back() == ')' ||
         splitAABB2.at( 2 ).front() == '(' ||
         splitAABB2.at( 3 ).back() == ')' )
    {
        // Remove the '(' or ')' from the beginning or ending of the string
        value.mins.x = stof( splitAABB2.at( 0 ).substr( 1 ) );
        value.mins.y = stof( splitAABB2.at( 1 ).substr( 0, splitAABB2.at( 1 ).size() - 1 ) );
        value.maxs.x = stof( splitAABB2.at( 2 ).substr( 1 ) );
        value.maxs.y = stof( splitAABB2.at( 3 ).substr( 0, splitAABB2.at( 3 ).size() - 1 ) );
    }
    else
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "AABB2::CreateFromString - Received invalid sequence \"%s\"", data ) );
    #endif // !defined( ENGINE_DISABLE_CONSOLE )
    }

    return value;
}

const Vec2 AABB2::GetCenter() const
{
    return GetPointAtUV( Vec2( .5f, .5f ) );
}

const Vec2 AABB2::GetDimensions() const
{
    return Vec2( maxs.x - mins.x, maxs.y - mins.y );
}

const Vec2 AABB2::GetTopLeft() const
{
    return Vec2( mins.x, maxs.y );
}

const Vec2 AABB2::GetBotRight() const
{
    return Vec2( maxs.x, mins.y );
}

void AABB2::GetCornerPoints( Vec2* output ) const
{
    output[ 0 ] = maxs;
    output[ 1 ] = GetTopLeft();
    output[ 2 ] = mins;
    output[ 3 ] = GetBotRight();
}

const AABB2 AABB2::GetTranslation( const Vec2& translation ) const
{
    return AABB2( mins + translation, maxs + translation );
}

const AABB2 AABB2::GetScaled( const Vec2& scale )  const
{
    AABB2 copy = *this;
    copy.SetDimensions( copy.GetDimensions() * scale );
    return copy;
}

bool AABB2::IsPointInside( const Vec2& point ) const
{
    return (point.x >= mins.x && point.x <= maxs.x &&
             point.y >= mins.y && point.y <= maxs.y);
}

Vec2 AABB2::GetNearestPointInAABB2( const Vec2& from ) const
{
    return Vec2( Clamp( from.x, mins.x, maxs.x ),
                 Clamp( from.y, mins.y, maxs.y ) );
}

Vec2 AABB2::GetPointAtUV( const Vec2& uvPoint ) const
{
    return Vec2( Interpolate( mins.x, maxs.x, uvPoint.x ),
                 Interpolate( mins.y, maxs.y, uvPoint.y ) );
}

Vec2 AABB2::GetUVAtPoint( const Vec2& point ) const
{
    return Vec2( RangeMapFloat( mins.x, maxs.x, 0.f, 1.f, point.x ),
                 RangeMapFloat( mins.y, maxs.y, 0.f, 1.f, point.y ) );
}

const AABB2 AABB2::GetBoxFromRight( float percent, float extra ) const
{
    float xDimen = (maxs.x - mins.x) * percent;
    return AABB2( maxs.x - xDimen - extra, mins.y, maxs.x, maxs.y );
}

const AABB2 AABB2::GetBoxFromTop( float percent, float extra ) const
{
    float yDimen = (maxs.y - mins.y) * percent;
    return AABB2( mins.x, maxs.y - yDimen - extra, maxs.x, maxs.y );
}

const AABB2 AABB2::GetBoxFromLeft( float percent, float extra ) const
{
    float xDimen = (maxs.x - mins.x) * percent;
    return AABB2( mins.x, mins.y, mins.x + xDimen + extra, maxs.y );
}

const AABB2 AABB2::GetBoxFromBottom( float percent, float extra ) const
{
    float yDimen = (maxs.y - mins.y) * percent;
    return AABB2( mins.x, mins.y, maxs.x, mins.y + yDimen + extra );
}

void AABB2::SetCenter( const Vec2& newCenter )
{
    Vec2 halfDimen = GetDimensions() * .5f;
    mins = newCenter - halfDimen;
    maxs = newCenter + halfDimen;
}

void AABB2::SetDimensions( const Vec2& newDimension )
{
    Vec2 centerPoint = GetCenter();
    Vec2 halfDimen = newDimension * .5f;
    mins = centerPoint - halfDimen;
    maxs = centerPoint + halfDimen;
}

void AABB2::Translate( const Vec2& translation )
{
    mins += translation;
    maxs += translation;
}

void AABB2::TranslateCenter( const Vec2& translation )
{
    Vec2 centerPoint = GetCenter();
    SetCenter( translation + centerPoint );
}

void AABB2::ScaleFromMin( float uniformScale )
{
    ScaleFromMin( Vec2( uniformScale ) );
}

void AABB2::ScaleFromMin( const Vec2& scale )
{
    Vec2 scaledDimension = GetDimensions() * scale;
    maxs = mins + scaledDimension;
}

void AABB2::ScaleFromMax( float uniformScale )
{
    ScaleFromMax( Vec2( uniformScale ) );
}

void AABB2::ScaleFromMax( const Vec2& scale )
{
    Vec2 scaledDimension = GetDimensions() * scale;
    mins = maxs - scaledDimension;
}

void AABB2::ExpandToFitPoint( const Vec2& point )
{
    ExpandXToFit( point.x );
    ExpandYToFit( point.y );
}

void AABB2::FitWithinBounds( const AABB2& bounds )
{
    const Vec2 boundDimens = bounds.GetDimensions();
    const Vec2 myDimens = GetDimensions();
    const Vec2 diffDimens = myDimens - boundDimens;

    // Some scaling is needed
    if ( diffDimens.x > 0 || diffDimens.y > 0 )
    {
        float scaleByLargest = 1.0f;
        if ( diffDimens.x > diffDimens.y )
        {
            scaleByLargest = (myDimens.x - diffDimens.x) / myDimens.x;
        }
        else
        {
            scaleByLargest = (myDimens.y - diffDimens.y) / myDimens.y;
        }
        operator*=( scaleByLargest );
    }

    // Move To Fit
    if ( mins.x < bounds.mins.x )
    {
        Translate( Vec2( bounds.mins.x - mins.x, 0.f ) );
    }
    if ( maxs.x > bounds.maxs.x )
    {
        Translate( Vec2( bounds.maxs.x - maxs.x, 0.f ) );
    }
    if ( mins.y < bounds.mins.y )
    {
        Translate( Vec2( 0.f, bounds.mins.y - mins.y ) );
    }
    if ( maxs.y > bounds.maxs.y )
    {
        Translate( Vec2( 0.f, bounds.maxs.y - maxs.y ) );
    }
}

void AABB2::ScaleToFitWithinBounds( const AABB2& bounds, const Vec2& alignment )
{
    const Vec2 boundDimens = bounds.GetDimensions();
    const Vec2 myDimens = GetDimensions();
    const Vec2 diffDimens = myDimens - boundDimens;

    float scaleByLargest = 1.0f;
    if( diffDimens.x > diffDimens.y )
    {
        scaleByLargest = ( myDimens.x - diffDimens.x ) / myDimens.x;
    }
    else
    {
        scaleByLargest = ( myDimens.y - diffDimens.y ) / myDimens.y;
    }
    operator*=( scaleByLargest );

    if( mins.x < bounds.mins.x )
    {
        Translate( Vec2( bounds.mins.x - mins.x, 0.f ) );
    }
    if( maxs.x > bounds.maxs.x )
    {
        Translate( Vec2( bounds.maxs.x - maxs.x, 0.f ) );
    }
    if( mins.y < bounds.mins.y )
    {
        Translate( Vec2( 0.f, bounds.mins.y - mins.y ) );
    }
    if( maxs.y > bounds.maxs.y )
    {
        Translate( Vec2( 0.f, bounds.maxs.y - maxs.y ) );
    }

    AlignWithinBounds( bounds, alignment );
}

void AABB2::AlignWithinBounds( const AABB2& bounds, const Vec2& alignment )
{
    Vec2 currentDimension = GetDimensions();
    Vec2 extraDimensions = bounds.GetDimensions() - currentDimension;

    Vec2 bottomLeft = extraDimensions * alignment;

    mins = bounds.mins + bottomLeft;
    maxs = mins + currentDimension;
}

const AABB2 AABB2::CarveBoxFromRight( float percent, float extra )
{
    float xDimen = (maxs.x - mins.x);
    float carveSize = (xDimen * percent) + extra;
    float oldMaxX = maxs.x;

    if ( xDimen < carveSize )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_WARNING,
                        Stringf( "AABB2.CarveBoxFromRight resulting dimension will be < 0 given %.2f%% + %f against %f dimension",
                                 percent,
                                 extra,
                                 xDimen ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)

        maxs.x = mins.x;
    }
    else
    {
        maxs.x -= carveSize;
    }


    return AABB2( maxs.x, mins.y, oldMaxX, maxs.y );
}

const AABB2 AABB2::CarveBoxFromTop( float percent, float extra )
{
    float yDimen = (maxs.y - mins.y);
    float carveSize = (yDimen * percent) + extra;
    float oldMaxY = maxs.y;

    if ( yDimen < carveSize )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_WARNING,
                        Stringf( "AABB2.CarveBoxFromTop resulting dimension will be < 0 given %.2f%% + %f against %f dimension",
                                 percent,
                                 extra,
                                 yDimen ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)

        maxs.y = mins.y;
    }
    else
    {
        maxs.y -= carveSize;
    }

    return AABB2( mins.x, maxs.y, maxs.x, oldMaxY );
}

const AABB2 AABB2::CarveBoxFromLeft( float percent, float extra )
{
    float xDimen = (maxs.x - mins.x);
    float carveSize = (xDimen * percent) + extra;
    float oldMinX = mins.x;

    if ( xDimen < carveSize )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_WARNING,
                        Stringf( "AABB2.CarveBoxFromLeft resulting dimension will be < 0 given %.2f%% + %f against %f dimension",
                                 percent,
                                 extra,
                                 xDimen ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)

        mins.x = maxs.x;
    }
    else
    {
        mins.x += carveSize;
    }

    return AABB2( oldMinX, mins.y, mins.x, maxs.y );
}

const AABB2 AABB2::CarveBoxFromBottom( float percent, float extra )
{
    float yDimen = (maxs.y - mins.y);
    float carveSize = (yDimen * percent) + extra;
    float oldMinY = mins.y;

    if ( yDimen < carveSize )
    {
    #if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_WARNING,
                        Stringf( "AABB2.CarveBoxFromBottom resulting dimension will be < 0 given %.2f%% + %f against %f dimension",
                                 percent,
                                 extra,
                                 yDimen ) );
    #endif // !defined(ENGINE_DISABLE_CONSOLE)

        mins.y = maxs.y;
    }
    else
    {
        mins.y += carveSize;
    }

    return AABB2( mins.x, oldMinY, maxs.x, mins.y );
}

bool AABB2::operator==( const AABB2& compare ) const
{
    return mins == compare.mins && maxs == compare.maxs;
}

bool AABB2::operator!=( const AABB2& compare ) const
{
    return mins != compare.mins || maxs != compare.maxs;
}

void AABB2::operator*=( float uniformScale )
{
    Vec2 center = GetPointAtUV( Vec2( .5f, .5f ) );
    Vec2 uniformDisplace = (maxs - center) * uniformScale;

    mins = center - uniformDisplace;
    maxs = center + uniformDisplace;
}

void AABB2::operator*=( const Vec2& scale )
{
    Vec2 center = GetPointAtUV( Vec2( .5f, .5f ) );
    Vec2 scaledDisplace = (maxs - center) * scale;

    mins = center - scaledDisplace;
    maxs = center + scaledDisplace;
}

AABB2& AABB2::operator=( const AABB2& copyFrom )
{
    if ( this == &copyFrom ) { return *this; }

    mins = copyFrom.mins;
    maxs = copyFrom.maxs;

    return *this;
}

const Disc AABB2::GetInnerDisc() const
{
    const Vec2& dimensions = GetDimensions();
    float innerRadius = std::min( dimensions.x, dimensions.y ) * .5f;
    return Disc( GetCenter(), innerRadius );
}

const Disc AABB2::GetOuterDisc() const
{
    const Vec2& dimensions = GetDimensions();
    float outerRadius = dimensions.x + dimensions.y;
    return Disc( GetCenter(), outerRadius * .5f );
}

bool AABB2::CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float radius ) const
{
    // Check East Axis
    const Vec2& center = GetCenter();
    const Vec2& boxDimens = GetDimensions();
    float halfXDimen = boxDimens.x * .5f;
    FloatRange boxRange = FloatRange( -halfXDimen, halfXDimen );
    FloatRange pointsRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, center, Vec2::UNIT_EAST, radius );
    if ( !boxRange.DoesOverlap( pointsRange ) )
    {
        return true;
    }

    // Check North Axis
    float halfYDimen = boxDimens.y * .5f;
    boxRange = FloatRange( -halfYDimen, halfYDimen );
    pointsRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, center, Vec2::UNIT_NORTH, radius );
    if ( !boxRange.DoesOverlap( pointsRange ) )
    {
        return true;
    }

    return false;
}

bool AABB2::GetRayCrossRange( OUT_PARAM FloatRange& result, const Vec2& start, const Vec2& end ) const
{
    const Vec2 displacement = end - start;
    const Vec2 direction = displacement.GetNormalized();

    const float minXTime = ( mins.x - start.x ) / direction.x;
    const float maxXTime = ( maxs.x - start.x ) / direction.x;
    const float minYTime = ( mins.y - start.y ) / direction.y;
    const float maxYTime = ( maxs.y - start.y ) / direction.y;

    const float minTime = Maxf( Minf(minXTime, maxXTime), Minf(minYTime, maxYTime) );
    const float maxTime = Minf( Maxf(minXTime, maxXTime), Maxf( minYTime, maxYTime) );

    if( maxTime < 0 ) { return false; }
    if( minTime > maxTime ) { return false; }

    result.minimum = minTime;
    result.maximum = maxTime; 
    return true;
}

const Vec2 AABB2::RollRandomPointInside( RandomNumberGenerator& rng ) const
{
    return Vec2( rng.FloatInRange( mins.x, maxs.x ), rng.FloatInRange( mins.y, maxs.y ) );
}

void AABB2::ExpandXToFit( float x )
{
    if ( mins.x > x )
    {
        mins.x = x;
    }
    if ( maxs.x < x )
    {
        maxs.x = x;
    }
}

void AABB2::ExpandYToFit( float y )
{
    if ( mins.y > y )
    {
        mins.y = y;
    }
    if ( maxs.y < y )
    {
        maxs.y = y;
    }
}

STATIC const AABB2 AABB2::UNIT_BOX = AABB2( 0.f, 0.f, 1.f, 1.f );
STATIC const AABB2 AABB2::UNIT_AROUND_ZERO = AABB2( -.5f, -.5f, .5f, .5f );

std::string ConvertToString( const AABB2& value )
{
    return Stringf( "((%f,%f),(%f,%f))", value.mins.x, value.mins.y, value.maxs.x, value.maxs.y );
}

std::string GetTypeName( const AABB2& value )
{
    UNUSED( value );
    return std::string( "AABB2" );
}

/* Convert AABB2 from a formatted string
 - Format ((mins.x, mins.y), (maxs.x, maxs.y)) or (mins.x, mins.y, maxs.x, maxs.y)
           (mins.x, mins.y), (maxs.x, maxs.y)  or  mins.x, mins.y, maxs.x, maxs.y
 */
AABB2 ConvertFromString( const char* string, const AABB2& defaultValue )
{
    return ConvertFromString( std::string(string), defaultValue );
}

/* Convert AABB2 from a formatted string
 - Format ((mins.x, mins.y), (maxs.x, maxs.y)) or (mins.x, mins.y, maxs.x, maxs.y)
           (mins.x, mins.y), (maxs.x, maxs.y)  or  mins.x, mins.y, maxs.x, maxs.y
 */
AABB2 ConvertFromString( const std::string& string, const AABB2& defaultValue )
{
    AABB2 result = defaultValue;
    const Strings split = SplitStringOnDelimiter( string, ',' );
    if( split.size() == 4 )
    {
        bool enclosedTotal = false;
        bool enclosedMins = false;
        bool enclosedMaxs = false;

        // If parameter is enclosed
        if( split.at( 0 ).front() == '(' )
        {
            // If mins is enclosed
            if( split.at( 0 ).at( 1 )  == '(' )
            {
                result.mins.x = ConvertFromString( split.at( 0 ).substr( 2 ), defaultValue.mins.x);
                enclosedMins = true;
                enclosedTotal = true;
            }
            else
            {
                if( split.at( 1 ).back() == ')' )
                {
                    enclosedMins = true;
                }
                else
                {
                    enclosedTotal = false;
                }
                result.mins.x = ConvertFromString( split.at( 0 ).substr( 1 ), defaultValue.mins.x);
            }
        }
        else
        {
            // If mins is enclosed
            if ( split.at( 0 ).at( 0 ) == '('  )
            {
                result.mins.x = ConvertFromString( split.at( 0 ).substr( 1 ), defaultValue.mins.x );
                enclosedMins = true;
            }
            else
            {
                result.mins.x = ConvertFromString( split.at( 0 ), defaultValue.mins.x );
            }
        }

        if( enclosedMins )
        {
            if( split.at(1).back() == ')' )
            {
                const size_t count = split.at( 1 ).size() - 1;
                result.mins.y = ConvertFromString( split.at( 1 ).substr( 0, count ), defaultValue.mins.y);
            }
            else
            {
                ERROR_RECOVERABLE( Stringf( "AABB2::ConvertFromString - No ending brace parsing %s", string.c_str() ) );
                return defaultValue;
            }
        }
        else
        {
            result.mins.y = ConvertFromString( split.at( 1 ), defaultValue.mins.y );
        }

        if( split.at( 2 ).front() == '(' )
        {
            result.maxs.x = ConvertFromString( split.at( 2 ).substr( 1 ), defaultValue.maxs.x);
            enclosedMaxs = true;
        }
        else
        {
            result.maxs.x = ConvertFromString( split.at( 2 ), defaultValue.maxs.x);
        }

        if( enclosedTotal )
        {
            if( enclosedMaxs )
            {
                if ( split.at( 3 ).back() == ')' && 
                     split.at( 3 ).at( split.at( 3 ).size() - 2 ) == ')' )
                {
                    const size_t count = split.at( 3 ).size() - 2;
                    result.maxs.y = ConvertFromString( split.at( 3 ).substr( 0, count ), defaultValue.maxs.y );
                }
                else
                {
                    ERROR_RECOVERABLE( Stringf( "AABB2::ConvertFromString - No ending brace parsing %s", string.c_str() ) );
                    return defaultValue;
                }
            }
            else
            {
                if( split.at( 3 ).back() == ')' )
                {
                    const size_t count = split.at( 3 ).size() - 1;
                    result.maxs.y = ConvertFromString( split.at( 3 ).substr( 0, count ), defaultValue.maxs.y);
                }
                else
                {
                    ERROR_RECOVERABLE( Stringf( "AABB2::ConvertFromString - No ending brace parsing %s", string.c_str() ) );
                    return defaultValue;
                }
            }
        }
        else
        {
            if( enclosedMaxs )
            {
                if ( split.at( 3 ).back() == ')' )
                {
                    const size_t count = split.at( 3 ).size() - 1;
                    result.maxs.y = ConvertFromString( split.at( 3 ).substr( 0, count ), defaultValue.maxs.y );
                }
                else
                {
                    ERROR_RECOVERABLE( Stringf( "AABB2::ConvertFromString - No ending brace parsing %s", string.c_str() ) );
                    return defaultValue;
                }
            }
            else
            {
                result.maxs.y = ConvertFromString( split.at( 3 ), defaultValue.maxs.y);
            }
        }
    }

    return result;
}
