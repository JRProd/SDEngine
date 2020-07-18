#pragma once
#include "Vec3.hpp"

struct LineSeg2D;

struct LineSeg3D
{
    Vec3 start = Vec3::ZERO;
    Vec3 end = Vec3::ZERO;

public:
    LineSeg3D() = default;

    explicit LineSeg3D( float startX, float startY, float startZ, float endX, float endY, float endZ );
    explicit LineSeg3D( const Vec3& lineStart, const Vec3& lineEnd );
    explicit LineSeg3D( const LineSeg2D& lineSeg );

    float GetLength() const;
    Vec3 GetDisplacement() const;
    Vec3 GetDirection() const;
};
