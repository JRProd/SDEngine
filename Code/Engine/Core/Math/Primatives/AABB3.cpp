#include "AABB3.hpp"


#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

AABB3::AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
    : mins( minX, minY, minZ )
      , maxs( maxX, maxY, maxZ )
{
}

AABB3::AABB3( const Vec3& min, const Vec3& max )
    : mins( min )
      , maxs( max )
{
}

AABB3::AABB3( const Vec3& center, float size )
    : mins( center - Vec3( size, size, size ) )
      , maxs( center + Vec3( size, size, size ) )
{
}

AABB3::AABB3( const IntVec3& min, const IntVec3& max )
{
    UNUSED( min );
    UNUSED( max );

    UNIMPLEMENTED();
}

bool AABB3::IsPointInside( const Vec3& point ) const
{
    if ( point.x < mins.x || point.x > maxs.x ) { return false; }
    if ( point.y < mins.y || point.y > maxs.y ) { return false; }
    if ( point.z < mins.z || point.z > maxs.z ) { return false; }
    return true;
}

Vec3 AABB3::GetCenter() const
{
    return (mins + maxs) * .5f;
}

Vec3 AABB3::GetDimensions() const
{
    return maxs - mins;
}

void AABB3::SetCenter( const Vec3& newCenter )
{
    const Vec3 dimension = GetDimensions();

    mins = newCenter - dimension;
    maxs = newCenter + dimension;
}
