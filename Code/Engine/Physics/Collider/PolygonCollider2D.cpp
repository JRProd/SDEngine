#include "PolygonCollider2D.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"
#include "Engine/Physics/Collider/DiscCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Vec2 PolygonCollider2D::GetClosestPoint( const Vec2& point ) const
{
    // Find all the line segments that the point is on the wrong side of
    const std::vector<Vec2> worldPoints = GetPoints();
    std::vector<LineSeg2D> potentialLineSegs;
    for ( size_t currentStartIndex = 0; currentStartIndex < worldPoints.size() - 1; ++
          currentStartIndex )
    {
        LineSeg2D currentLineSeg = LineSeg2D( worldPoints.at( currentStartIndex ),
                                              worldPoints.at( currentStartIndex + 1 ) );
        if ( PointOnNegativeSide( currentLineSeg, point ) )
        {
            potentialLineSegs.push_back( currentLineSeg );
        }
    }

    const LineSeg2D endToStart( worldPoints.at( worldPoints.size() - 1 ),
                                worldPoints.at( 0 ) );
    if ( PointOnNegativeSide( endToStart, point ) )
    {
        potentialLineSegs.push_back( endToStart );
    }

    // Either this point is the closest, or there is another closest point
    if ( !potentialLineSegs.empty() )
    {
        Vec2 closestPoint = potentialLineSegs.at( 0 ).start;
        for ( const LineSeg2D& lineSeg : potentialLineSegs )
        {
            Vec2 potentialClosestPoint = lineSeg.GetNearestPointOnLineSeg2D( point );
            if ( Vec2::GetDistanceSquared( point, potentialClosestPoint ) < Vec2::
                GetDistanceSquared(
                    point, closestPoint ) )
            {
                closestPoint = potentialClosestPoint;
            }
        }
        return closestPoint;
    }
    return point;
}

Vec2 PolygonCollider2D::GetClosestPointOnHull( const Vec2& point ) const
{
    const std::vector<Vec2> worldPoint = GetPoints();
    std::vector<LineSeg2D> lineSegs;

    for ( size_t pointIndex = 0; pointIndex < worldPoint.size() - 1; ++pointIndex )
    {
        lineSegs.emplace_back( worldPoint.at( pointIndex ), worldPoint.at( pointIndex + 1 ) );
    }
    const LineSeg2D endToStart( worldPoint.at( worldPoint.size() - 1 ),
                                worldPoint.at( 0 ) );
    lineSegs.push_back( endToStart );

    // Either this point is the closest, or there is another closest point
    Vec2 closestPoint = lineSegs.at( 0 ).start;
    for ( const LineSeg2D& lineSeg : lineSegs )
    {
        Vec2 potentialClosestPoint = lineSeg.GetNearestPointOnLineSeg2D( point );
        if ( Vec2::GetDistanceSquared( point, potentialClosestPoint ) < Vec2::
             GetDistanceSquared(
                 point, closestPoint ) )
        {
            closestPoint = potentialClosestPoint;
        }
    }
    return closestPoint;
}

bool PolygonCollider2D::Contains( const Vec2& point ) const
{
    return GetClosestPoint( point ) == point;
}

void PolygonCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor,
                                     const Rgba8& fillColor ) const
{
    std::vector<VertexMaster> polygonVisual;

    const std::vector<Vec2> m_WorldPoints = GetPoints();

    AppendPolygon2D( polygonVisual, m_WorldPoints, fillColor );
    AppendPolygon2DPerimeter( polygonVisual, m_WorldPoints, borderColor, .25f );

    ctx->BindTexture( nullptr );
    ctx->DrawVertexArray( polygonVisual );
}

float PolygonCollider2D::CalculateMoment( float mass ) const
{
    return mass * m_MasslessMoment;
}

Vec2 PolygonCollider2D::GetSupportPoint( const Vec2& direction ) const
{
    const Mat44 localToWorld = Mat44::CreateFromZRotationDegrees( m_Rigidbody->GetAngleDegrees() );
    Vec2 furthestPoint;
    float furthestPointDot = 0.f;   // Don't need to worry about negatives

    for( const Vec2& point: m_LocalPoints )
    {
        const Vec2 translated = localToWorld.TransformVector( point );
        const float dot = Vec2::Dot( direction, translated );
        if( dot > furthestPointDot )
        {
            furthestPoint = translated;
            furthestPointDot = dot;
        }
    }

    return furthestPoint + m_WorldPosition;
}

Vec2 PolygonCollider2D::GetRightMostPoint() const
{
    return GetDirectionMostPoint( 0 );
}

Vec2 PolygonCollider2D::GetTopMostPoint() const
{
    return GetDirectionMostPoint( 1 );
}

Vec2 PolygonCollider2D::GetLeftMostPoint() const
{
    return GetDirectionMostPoint( 2 );
}

Vec2 PolygonCollider2D::GetBottomMostPoint() const
{
    return GetDirectionMostPoint( 3 );
}

std::vector<Vec2> PolygonCollider2D::GetPoints() const
{
    Mat44 localToWorld = Mat44::CreateFromZRotationDegrees( m_Rigidbody->GetAngleDegrees() );
    localToWorld.SetTranslation( m_WorldPosition );
    std::vector<Vec2> points;
    for ( const Vec2& point : m_LocalPoints )
    {
        points.push_back( localToWorld.TransformPosition(point) );
    }

    return points;
}

Disc PolygonCollider2D::GetWorldBounds() const
{
    Mat44 localToWorld = Mat44::CreateFromZRotationDegrees( m_Rigidbody->GetAngleDegrees() );
    localToWorld.SetTranslation( m_WorldPosition );
    Disc worldBounds;
    worldBounds.center = localToWorld.TransformPosition(m_LocalBounds.center);
    worldBounds.radius = m_LocalBounds.radius;
    return worldBounds;
}

PolygonCollider2D::PolygonCollider2D( Physics2D* physicsWorld, const std::vector<Vec2>& points, bool isCloud )
    : Collider2D( physicsWorld )
{
    m_Type = COLLIDER_POLYGON;

    if ( isCloud )
    {
        BuildFromCloud( points );
    }
    else
    {
        BuildFromInOrder( points );
    }

    FindCenterOfMassAndMoment();
    LocalizePoints();
    CalculateLocalBounds();
}

void PolygonCollider2D::BuildFromInOrder( const std::vector<Vec2>& points )
{
    Vec2 lastPoint;
    for ( size_t currentPointIndex = 0; currentPointIndex < points.size() - 1; ++currentPointIndex )
    {
        const Vec2& startPoint = points.at( currentPointIndex );
        const Vec2& endPoint = points.at( currentPointIndex + 1 );

        if ( currentPointIndex == 0 )
        {
            m_LocalPoints.push_back( startPoint );
        }
        else
        {
            const bool isValid = !PointOnNegativeSide( LineSeg2D( lastPoint, startPoint ), endPoint );
            GUARANTEE_OR_DIE(
                isValid,
                "PolygonCollider2D: Points are not in counter-clockwise order. Failed to create polygon" );
        }
        m_LocalPoints.push_back( endPoint );
        lastPoint = startPoint;
    }
}

void PolygonCollider2D::BuildFromCloud( const std::vector<Vec2>& points )
{
    // Gift Wrapping Algorithm
    //  Find the left most point
    Vec2 pointOnHull = points.at( 0 );
    for ( const Vec2& testPoint : points )
    {
        if ( pointOnHull.x > testPoint.x )

        {
            pointOnHull = testPoint;
        }
    }

    int currentConvexHullPoint = 0;
    Vec2 endpoint = points.at( 0 );
    do
    {
        m_LocalPoints.push_back( pointOnHull );

        for ( const Vec2& nextPotentialEndpoint : points )
        {
            if ( endpoint == pointOnHull ||
                PointOnNegativeSide(
                    LineSeg2D( m_LocalPoints.at( currentConvexHullPoint ), endpoint ),
                    nextPotentialEndpoint, true ) )
            {
                endpoint = nextPotentialEndpoint;
            }
        }
        currentConvexHullPoint++;
        pointOnHull = endpoint;
    }
    while ( endpoint != m_LocalPoints.at( 0 ) );
}

void PolygonCollider2D::FindCenterOfMassAndMoment()
{
    const int triangleCount = static_cast<int>(m_LocalPoints.size()) - 2;
    GUARANTEE_OR_DIE( triangleCount > 0, "There must be one or more triangles" );

    float* triangleAreas = new float[ triangleCount ];
    float* triangleMoments = new float[ triangleCount ];

    float totalArea = 0.f;

    constexpr float third = 1.f / 3.f;
    const Vec2& zeroPoint = m_LocalPoints.at( 0 );
    for ( size_t triangulationIndex = 1; triangulationIndex < m_LocalPoints.size() - 1; ++
          triangulationIndex )
    {
        Vec2 u = m_LocalPoints.at( triangulationIndex );
        Vec2 v = m_LocalPoints.at( triangulationIndex + 1 );
        const Vec2 triangleCenter = (zeroPoint + u + v) * third;
        m_LocalTriangleCenters.push_back( triangleCenter );
        u = u - zeroPoint;
        v = v - zeroPoint;

        const float b = u.GetLength();
        const float h = v.GetProjectedLength( u.GetRotated90Degrees() );
        const float area = b * h * .5f;

        const float uu = Vec2::Dot( u, u );
        const float uv = Vec2::Dot( u, v );
        float moment = 1.f / 18.f;
        moment *= (uu - uv + (uv * uv / uu) + h * h);

        
        triangleAreas[ triangulationIndex - 1 ] = area;
        triangleMoments[ triangulationIndex - 1 ] = moment;

        m_WorldPosition += triangleCenter * area;

        totalArea += area;
    }

    m_WorldPosition /= totalArea;

    for ( int triangleIndex = 0; triangleIndex < m_LocalTriangleCenters.size(); ++triangleIndex )
    {
        // Localizes the triangle centers
        Vec2& triangleCenter = m_LocalTriangleCenters.at( triangleIndex );
        triangleCenter -= m_WorldPosition;

        // Calculate the moment based on area
        const float parrallelAxis = triangleAreas[ triangleIndex ] * Vec2::Dot( triangleCenter, triangleCenter );
        m_MasslessMoment += triangleMoments[ triangleIndex ] + parrallelAxis;
    }
    m_MasslessMoment /= totalArea;

    delete[] triangleAreas;
    delete[] triangleMoments;
}

void PolygonCollider2D::LocalizePoints()
{
    for( Vec2& localPoint : m_LocalPoints )
    {
        localPoint -= m_WorldPosition;
    }
}

void PolygonCollider2D::CalculateLocalBounds()
{
    Disc smallestTightlyBound = Disc( m_LocalPoints.at( 0 ), m_LocalPoints.at( 1 ) );
    bool isSmallestValid = false;

    const size_t numberOfPoints = m_LocalPoints.size();
    for ( size_t pointOneIndex = 0; pointOneIndex < numberOfPoints; ++pointOneIndex )
    {
        const Vec2& pointOne = m_LocalPoints.at( pointOneIndex );

        for ( size_t pointTwoIndex = pointOneIndex + 1; pointTwoIndex < numberOfPoints; ++pointTwoIndex
        )
        {
            const Vec2& pointTwo = m_LocalPoints.at( pointTwoIndex );
            Disc twoPointDisc( pointOne, pointTwo );
            bool isTwoPointDiscValid = true;

            for ( size_t pointFourIndex = 0; pointFourIndex < numberOfPoints; ++
                  pointFourIndex )
            {
                if ( pointFourIndex == pointOneIndex || pointFourIndex == pointTwoIndex )
                {
                    continue;
                }

                const Vec2& pointFour = m_LocalPoints.at( pointFourIndex );
                if ( !twoPointDisc.IsPointInside( pointFour ) )
                {
                    isTwoPointDiscValid = false;
                    break;
                }
            }

            for ( size_t pointThreeIndex = pointTwoIndex + 1; pointThreeIndex < numberOfPoints; ++
                  pointThreeIndex )
            {
                const Vec2& pointThree = m_LocalPoints.at( pointThreeIndex );
                Disc threePointDisc( pointOne, pointTwo, pointThree );
                bool isThreePointDiscValid = true;


                if ( !twoPointDisc.IsPointInside( pointThree ) )
                {
                    isTwoPointDiscValid = false;
                }

                for ( size_t pointFourIndex = 0; pointFourIndex < numberOfPoints; ++
                      pointFourIndex )
                {
                    if ( pointFourIndex == pointOneIndex || pointFourIndex == pointTwoIndex ||
                        pointFourIndex == pointThreeIndex )
                    {
                        continue;
                    }
                    const Vec2& pointFour = m_LocalPoints.at( pointFourIndex );
                    if ( !threePointDisc.IsPointInside( pointFour ) )
                    {
                        isThreePointDiscValid = false;
                        break;
                    }
                }

                if ( isSmallestValid )
                {
                    if ( isThreePointDiscValid && smallestTightlyBound.radius > threePointDisc.
                        radius )
                    {
                        smallestTightlyBound = threePointDisc;
                    }
                }
                else if ( isThreePointDiscValid )
                {
                    smallestTightlyBound = threePointDisc;
                    isSmallestValid = true;
                }
            }

            if ( isSmallestValid )
            {
                if ( isTwoPointDiscValid && smallestTightlyBound.radius > twoPointDisc.radius )
                {
                    smallestTightlyBound = twoPointDisc;
                }
            }
            else if ( isTwoPointDiscValid )
            {
                smallestTightlyBound = twoPointDisc;
                isSmallestValid = true;
            }
        }
    }

    m_LocalBounds = smallestTightlyBound;
}

STATIC bool PolygonCollider2D::PointOnNegativeSide( const LineSeg2D& lineSeg, const Vec2& testPoint,
                                                    bool isPositiveProjectionNegative )
{
    const Vec2 pointRelativeToStart = testPoint - lineSeg.start;
    if ( pointRelativeToStart == Vec2::ZERO ) { return false; }

    const float dot = Vec2::Dot( lineSeg.GetNormal(), pointRelativeToStart );
    if ( dot < 0.f )
    {
        return true;
    }
    if ( dot == 0.f )
    {
        const Vec2 pointRelativeToEnd = testPoint - lineSeg.end;
        const float length = pointRelativeToEnd.GetProjectedLength( lineSeg.GetDirection() );
        if ( length > 0.f )
        {
            return isPositiveProjectionNegative;
        }
    }
    return false;
}

Vec2 PolygonCollider2D::GetDirectionMostPoint( int direction ) const
{
    Vec2 directionMostPoint = m_LocalPoints.at( 0 );

    if( direction == 0 ) // Test right most
    {
        for ( const Vec2& testPoint : m_LocalPoints )
        {
            if (directionMostPoint.x < testPoint.x ) 
            {
                directionMostPoint = testPoint;
            }
        }
        
    }
    else if ( direction == 1 ) // Test top most
    {
        for ( const Vec2& testPoint : m_LocalPoints )
        {
            if ( directionMostPoint.y < testPoint.y ) 
            {
                directionMostPoint = testPoint;
            }
        }
    }
    else if ( direction == 2 ) // Test left most
    {
        for ( const Vec2& testPoint : m_LocalPoints )
        {
            if ( directionMostPoint.x > testPoint.x ) 
            {
                directionMostPoint = testPoint;
            }
        }
    }
    else if ( direction == 3 ) // Test bottom most
    {
        for ( const Vec2& testPoint : m_LocalPoints )
        {
            if ( directionMostPoint.y > testPoint.y ) 
            {
                directionMostPoint = testPoint;
            }
        }
    }

    return directionMostPoint;
}
