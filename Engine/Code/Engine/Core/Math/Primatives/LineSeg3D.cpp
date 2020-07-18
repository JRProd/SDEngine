#include "LineSeg3D.hpp"

#include "LineSeg2D.hpp"

LineSeg3D::LineSeg3D( const float startX, const float startY, const float startZ, const float endX,
                      const float endY, const float endZ )
    : start( startX, startY, startZ )
      , end( endX, endY, endZ )
{
}

LineSeg3D::LineSeg3D( const Vec3& lineStart, const Vec3& lineEnd )
    : start( lineStart )
      , end( lineEnd )
{
}

LineSeg3D::LineSeg3D( const LineSeg2D& lineSeg )
    : start( lineSeg.start )
      , end( lineSeg.end )
{
}

float LineSeg3D::GetLength() const
{
    return GetDisplacement().GetLength();
}

Vec3 LineSeg3D::GetDisplacement() const
{
    return end - start;
}

Vec3 LineSeg3D::GetDirection() const
{
    return GetDisplacement().GetNormalized();
}
