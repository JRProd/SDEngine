#include "OBB2.hpp"

#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Range/FloatRange.hpp"

#include <algorithm>

//-----------------------------------------------------------------------------
OBB2::OBB2( const OBB2& copyFrom )
    : center( copyFrom.center )
    , halfDimension( copyFrom.halfDimension )
    , iBasis( copyFrom.iBasis )
{
}

//-----------------------------------------------------------------------------
OBB2::OBB2( const Vec2& boxCenter, const Vec2& fullDimension, const Vec2& iBasisNormalized )
    : center( boxCenter )
    , halfDimension( fullDimension * .5f )
    , iBasis( iBasisNormalized )
{
}

//-----------------------------------------------------------------------------
OBB2::OBB2( const Vec2& boxCenter, const Vec2& fullDimension, float angleDegrees )
    : center( boxCenter )
    , halfDimension( fullDimension * .5f )
    , iBasis( Vec2::MakeFromPolarDegrees( angleDegrees ) )
{
}

//-----------------------------------------------------------------------------
OBB2::OBB2( const AABB2& aabb2, float angleDegrees )
    : center( aabb2.GetCenter() )
    , halfDimension( aabb2.GetDimensions() * .5f )
    , iBasis( Vec2::MakeFromPolarDegrees( angleDegrees ) )
{
}

/*
bool OBB2::DoOBB2sOverlap( const OBB2& box1, const OBB2& box2 )
{
    // Check the outer radius first
    if ( !Disc::DoDiscsOverlap( box1.GetOuterDisc(), box2.GetOuterDisc() ) )
    {
        return false;
    }

    //Check the inner radius
    if ( Disc::DoDiscsOverlap( box1.GetInnerDisc(), box2.GetInnerDisc() ) )
    {
        return true;
    }

    if ( box1.FindSeparatingAxis( box2 ) )
    {
        return false;
    }
    if ( box2.FindSeparatingAxis( box1 ) )
    {
        return false;
    }
    return true;
}
*/

const Disc OBB2::GetInnerDisc() const
{
    float innerRadius = std::min( halfDimension.x, halfDimension.y );
    return Disc(center, innerRadius);
}

const Disc OBB2::GetOuterDisc() const
{
    return Disc(center, halfDimension.x + halfDimension.y);
}

bool OBB2::IsPointInside( const Vec2& testPoint ) const
{
    return testPoint == GetNearestPoint(testPoint);
}

const Vec2 OBB2::GetNearestPoint( const Vec2& testPoint ) const
{
    Vec2 testPointLocal = GetPointInLocalSpace( testPoint );
    testPointLocal = Vec2::ClampVector( testPointLocal, 
                                        -halfDimension, 
                                        halfDimension );
    return GetPointInWorldSpace( testPointLocal );
}

bool OBB2::CheckSimpleSeparationAgainstPoints( int numPoints, Vec2* points, float radius ) const
{
    FloatRange myRange = FloatRange( -halfDimension.x, halfDimension.x );
    FloatRange boxRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, center, iBasis, radius );
    if ( !myRange.DoesOverlap( boxRange ) )
    {
        return true;
    }

    myRange = FloatRange( -halfDimension.y, halfDimension.y );
    boxRange = FloatRange::CreateFromPointsOnAxis( numPoints, points, center, GetJBasis(), radius );
    if ( !myRange.DoesOverlap( boxRange ) )
    {
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
const Vec2 OBB2::GetPointAtUV( const Vec2& uvPoint ) const
{
    Vec2 localPoint = uvPoint;
    localPoint.x = RangeMapFloat( 0.f, 1.f, 
                                  -halfDimension.x, halfDimension.x,
                                  localPoint.x );
    localPoint.y = RangeMapFloat( 0.f, 1.f, 
                                  -halfDimension.y, halfDimension.y, 
                                  localPoint.y );
    return GetPointInWorldSpace(localPoint);
}

//-----------------------------------------------------------------------------
const Vec2 OBB2::GetUVForPoint( const Vec2& point ) const
{
    Vec2 localPoint = GetPointInLocalSpace( point );
    localPoint.x = RangeMapFloatZeroOne( -halfDimension.x, halfDimension.x, 
                                         localPoint.x );
    localPoint.y = RangeMapFloatZeroOne( -halfDimension.y, halfDimension.y,
                                         localPoint.y );
    return localPoint;
}

void OBB2::GetCornerPoints( Vec2* output ) const
{
    Vec2 xDisplacement = halfDimension.x * iBasis;
    Vec2 yDisplacement = halfDimension.y * GetJBasis();

    output[ 0 ] = center + xDisplacement + yDisplacement;
    output[ 1 ] = center - xDisplacement + yDisplacement;
    output[ 2 ] = center - xDisplacement - yDisplacement;
    output[ 3 ] = center + xDisplacement - yDisplacement;
}

//-----------------------------------------------------------------------------
void OBB2::SetCenter( const Vec2& newCenter )
{
    center = newCenter;
}

//-----------------------------------------------------------------------------
void OBB2::Translate( const Vec2& translation )
{
    center += translation;
}

//-----------------------------------------------------------------------------
void OBB2::SetDimension( const Vec2& newDimension )
{
    halfDimension = newDimension;
}

//-----------------------------------------------------------------------------
void OBB2::AddDimension( const Vec2& deltaDimension )
{
    halfDimension += deltaDimension;
    Fix();
}

//-----------------------------------------------------------------------------
void OBB2::SetOrientationDegrees( float newDegrees )
{
    iBasis = Vec2::MakeFromPolarDegrees( newDegrees );
}

//-----------------------------------------------------------------------------
void OBB2::RotateDegrees( float deltaDegrees )
{
    iBasis = Vec2::MakeFromPolarDegrees( iBasis.GetAngleDegrees() + deltaDegrees );
}

void OBB2::Fix()
{
    halfDimension = Vec2( abs( halfDimension.x ), abs( halfDimension.y ) );

    // If iBasis is not normalized, normalize
    if ( iBasis.GetLengthSquared() != 1 )
    {
        iBasis.Normalize();
    }
    // If iBasis is zero, force it to (1.f, 0.f)
    if ( iBasis.GetLengthSquared() == 0 )
    {
        iBasis = Vec2::UNIT_EAST;
    }
}

//-----------------------------------------------------------------------------
const Vec2 OBB2::GetPointInLocalSpace( const Vec2& pointInWorld ) const
{
    Vec2 relativePoint = pointInWorld - center;
    return Vec2( Vec2::Dot( relativePoint, iBasis ), 
                 Vec2::Dot( relativePoint, GetJBasis() ) );
}

//-----------------------------------------------------------------------------
const Vec2 OBB2::GetPointInWorldSpace( const Vec2& pointInLocal ) const
{
    return (pointInLocal.x * iBasis) + (pointInLocal.y * GetJBasis()) + center;
}

/*
bool OBB2::FindSeparatingAxis( const OBB2& box ) const
{
    Vec2 points[ 4 ];
    GetCornerPoints( points );
    FloatRange myRange = FloatRange( -halfDimension.x, halfDimension.x );
    FloatRange myRangeTest = FloatRange::CreateFromPointsOnAxis( 4, points, center, iBasis );
    box.GetCornerPoints( points );
    FloatRange boxRange = FloatRange::CreateFromPointsOnAxis( 4, points, center, iBasis );
    if ( !myRange.DoesOverlap( boxRange ) )
    {
        return true;
    }

    myRange = FloatRange( -halfDimension.y, halfDimension.y );
    box.GetCornerPoints( points );
    boxRange = FloatRange::CreateFromPointsOnAxis( 4, points, center, GetJBasis() );
    if ( !myRange.DoesOverlap( boxRange ) )
    {
        return true;
    }

    return false;
}
*/
