#pragma once
#include "Vec3.hpp"

class IntVec3;

struct AABB3
{
    Vec3 mins;
    Vec3 maxs;

public:
    AABB3() = default;
    ~AABB3() = default;
    AABB3( const AABB3& copy ) = default;

    explicit AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ );
    explicit AABB3( const Vec3& min, const Vec3& max );
    explicit AABB3( const Vec3& center, float size );
    explicit AABB3( const IntVec3& min, const IntVec3& max );

    bool IsPointInside( const Vec3& point) const;
    Vec3 GetCenter() const;
    Vec3 GetDimensions() const;

    void SetCenter( const Vec3& newCenter );
};
