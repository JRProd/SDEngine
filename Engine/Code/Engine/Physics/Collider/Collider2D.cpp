#include "Collider2D.hpp"


#include "Engine/Core/Math/Primatives/Plane2D.hpp"
#include "Engine/Event/EventSystem.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider/Collision2D.hpp"
#include "Engine/Physics/Collider/DiscCollider2D.hpp"
#include "Engine/Physics/Collider/PolygonCollider2D.hpp"
#include "Engine/Renderer/DebugRenderer.hpp"

#include "Engine/Console/Console.hpp"
#include "Engine/Physics/Physics2D.hpp"

static bool g_ShowGJKDebug = false;
static int g_ShowGJKDebugLevel = 0;

bool CommandDebugCollisionGJK( EventArgs* args )
{
    const int numArgs = args->GetValue( "numArgs", 0 );
    if( numArgs == 0 )
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->InvalidArgument("debug_collision_GJK", "Must have at least one argument");
#endif // !defined(ENGINE_DISABLE_CONSOLE)
        return false;
    }

    std::vector<std::string> toggleKeywords;
    toggleKeywords.emplace_back( "toggle" );
    //g_ShowGJKDebug = args->GetValue( toggleKeywords, false );

    const std::string level = args->GetValue( "verbosity", "minimum" );
    if( level == "minimum" )
    {
        g_ShowGJKDebugLevel = 0;
    }
    else if( level == "verbose" )
    {
        g_ShowGJKDebugLevel = 1;
    }
    else if( level == "very" )
    {
        g_ShowGJKDebugLevel = 2;
    }
    else
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->InvalidArgument( "debug_collision_GJK", Stringf( "Invalid verbose level %s. Use \\[minimum, verbose, very\\]", level.c_str()) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Collision Manifold
typedef bool (*CollisionCheckCallback)( const Collider2D* colliderOne,
                                        const Collider2D* colliderTwo,
                                        OUT_PARAM Manifold2D& manifold );

static bool DiscVsDiscCollisionCallback( const Collider2D* colliderOne,
                                         const Collider2D* colliderTwo,
                                         OUT_PARAM Manifold2D& manifold )
{
    const Disc colliderOneDisc = colliderOne->GetWorldBounds();
    const Disc colliderTwoDisc = colliderTwo->GetWorldBounds();

    const bool intersects = Disc::Intersects( colliderOneDisc, colliderTwoDisc );
    // If its a trigger calculation only care if we intersect
    if( colliderOne->IsTrigger() || colliderTwo->IsTrigger() )
    {
        return intersects;
    }

    if ( intersects )
    {
        const Vec2 displacement = colliderOneDisc.center - colliderTwoDisc.center;
        manifold.penetration = (colliderOneDisc.radius + colliderTwoDisc.radius) - displacement.
            GetLength();
        manifold.normal = displacement.GetNormalized();

        const Vec2 edgePoint = colliderTwo->GetWorldPosition() + (manifold.normal * colliderTwoDisc
            .radius);
        manifold.contactEdge = LineSeg2D( edgePoint, edgePoint );
    }

    return intersects;
}

static bool DiscVsPolygonCollisionCallback( const Collider2D* disc, const Collider2D* polygon,
                                            OUT_PARAM Manifold2D& manifold )
{
    const DiscCollider2D* discCollider = dynamic_cast<const DiscCollider2D*>(disc);
    const PolygonCollider2D* polygonCollider = dynamic_cast<const PolygonCollider2D*>(polygon);
    const Vec2 closestPoint = polygonCollider->GetClosestPointOnHull(
        discCollider->GetWorldPosition() );
    const float radiusSquared = discCollider->m_LocalDisc.radius * discCollider->m_LocalDisc.radius;

    const bool intersects = Vec2::GetDistanceSquared( closestPoint,
                                                      discCollider->GetWorldPosition() ) < radiusSquared;

    // If its a trigger calculation only care if we intersect
    if ( disc->IsTrigger() || polygon->IsTrigger() )
    {
        const bool inside = polygonCollider->GetClosestPoint( discCollider->GetWorldPosition() ) == discCollider->GetWorldPosition();
        return intersects || inside;
    }

    if ( intersects )
    {
        Vec2 displacement = discCollider->GetWorldPosition() - closestPoint;
        const float length = displacement.NormalizeAndGetPreviousLength();

        manifold.contactEdge = LineSeg2D( closestPoint, closestPoint );
        manifold.penetration = discCollider->m_LocalDisc.radius - length;
        manifold.normal = displacement;

        if ( polygonCollider->Contains( discCollider->GetWorldPosition() ) )
        {
            manifold.penetration = discCollider->m_LocalDisc.radius + length;
            manifold.normal = -manifold.normal;
        }
    }

    return intersects;
}

static Vec2 GetMinkowskiSupport( const PolygonCollider2D& colliderOne,
                                 const PolygonCollider2D& colliderTwo, const Vec2& direction )
{
    return colliderOne.GetSupportPoint( direction ) - colliderTwo.GetSupportPoint( -direction );
}


static bool ExpandForOrigin( const PolygonCollider2D& colliderOne,
                             const PolygonCollider2D& colliderTwo,
                             OUT_PARAM std::vector<Vec2>& simplexPoints )
{
    static LineSeg2D simplexLines[ 3 ];
    while ( true )
    {
        // Create the three lines
        simplexLines[ 0 ] = LineSeg2D( simplexPoints[ 0 ], simplexPoints[ 1 ] );
        simplexLines[ 1 ] = LineSeg2D( simplexPoints[ 1 ], simplexPoints[ 2 ] );
        simplexLines[ 2 ] = LineSeg2D( simplexPoints[ 2 ], simplexPoints[ 0 ] );

        bool isOnPositive[ 3 ];
        isOnPositive[ 0 ] = simplexLines[ 0 ].IsOnPositiveSide( Vec2::ZERO );
        isOnPositive[ 1 ] = simplexLines[ 1 ].IsOnPositiveSide( Vec2::ZERO );
        isOnPositive[ 2 ] = simplexLines[ 2 ].IsOnPositiveSide( Vec2::ZERO );

        // Line from point 0 to point 1
        if ( !isOnPositive[ 0 ] )
        {
            const Vec2 direction = simplexLines[ 0 ].GetNormal() * -1.f;

            // Reorder the lines to still make a counter clockwise winding
            simplexPoints[ 2 ] = simplexPoints[ 1 ];
            simplexPoints[ 1 ] = GetMinkowskiSupport( colliderOne, colliderTwo, direction );

            // Negative simplexPoint is the displacement from the origin. We are checking if the
            // new simplex point goes past the origin
            const float originDot = Vec2::Dot( simplexPoints[ 1 ], direction );
            if ( originDot <= 0 || IsMostlyEqual( originDot, 0.f, 1e-5f ) )
            {
                return false;
            }
        }
            // Line from point 1 to point 2
        else if ( !isOnPositive[ 1 ] )
        {
            const Vec2 direction = simplexLines[ 1 ].GetNormal() * -1.f;

            // Reorder the lines to still make a counter clockwise winding
            simplexPoints[ 0 ] = simplexPoints[ 1 ];
            simplexPoints[ 1 ] = GetMinkowskiSupport( colliderOne, colliderTwo, direction );

            // Negative simplexPoint is the displacement from the origin. We are checking if the
            // new simplex point goes past the origin
            const float originDot = Vec2::Dot( simplexPoints[ 1 ], direction );
            if ( originDot <= 0 || IsMostlyEqual( originDot, 0.f, 1e-5f ) )
            {
                return false;
            }
        }
            // Line from point 2 to point 0
        else if ( !isOnPositive[ 2 ] )
        {
            const Vec2 direction = simplexLines[ 2 ].GetNormal() * -1.f;

            simplexPoints[ 1 ] = simplexPoints[ 2 ];
            simplexPoints[ 2 ] = GetMinkowskiSupport( colliderOne, colliderTwo, direction );

            // Negative simplexPoint is the displacement from the origin. We are checking if the
            // new simplex point goes past the origin
            const float originDot = Vec2::Dot( simplexPoints[ 2 ], direction );
            if ( originDot <= 0 || IsMostlyEqual( originDot, 0.f, 1e-5f ) )
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
}

static Vec2 CalculatePenetration( const PolygonCollider2D& colliderOne,
                                  const PolygonCollider2D& colliderTwo,
                                  OUT_PARAM std::vector<Vec2>& simplexPoints )
{
    float shortestDistance = 0.f;
    Vec2 closestNormal;
    for( int i = 0; i < 32; i++ )
    {
        shortestDistance = INFINITY;
        closestNormal = Vec2::ZERO;
        size_t indexOfShortest = 0;
        for ( size_t pointIndex = 0; pointIndex < simplexPoints.size(); ++pointIndex )
        {
            const Vec2 pointOne = simplexPoints.at( pointIndex );
            Vec2 pointTwo;
            if ( pointIndex < simplexPoints.size() - 1 )
            {
                pointTwo = simplexPoints.at( pointIndex + 1 );
            }
            else
            {
                pointTwo = simplexPoints.at( 0 );
            }

            const LineSeg2D lineCheck( pointOne, pointTwo );
            const float distanceToZero = lineCheck.GetDistanceToPoint( Vec2::ZERO );

            if ( distanceToZero < shortestDistance )
            {
                shortestDistance = distanceToZero;
                closestNormal = lineCheck.GetNormal();
                indexOfShortest = pointIndex;
            }
        }

        const Vec2 outsideNormal = closestNormal * -1.f;
        const Vec2 supportPoint = GetMinkowskiSupport( colliderOne, colliderTwo, outsideNormal );
        const float supportDist = Vec2::Dot( outsideNormal, supportPoint );

        if ( IsMostlyEqual( supportDist, shortestDistance ) )
        {
            return outsideNormal * -shortestDistance;
        }
        else
        {
            const std::vector<Vec2>::const_iterator ci = simplexPoints.cbegin() + indexOfShortest + 1;
            simplexPoints.insert( ci, supportPoint );
        }
    }

    return closestNormal * shortestDistance;
}

static LineSeg2D GetReferenceEdge( const PolygonCollider2D& polygonCollider, const Plane2D& clipPlane )
{
    const Vec2 tangent = clipPlane.GetTangent();
    float bMin = INFINITY;
    float bMax = -INFINITY;

    for ( const Vec2& colliderTwoPoint : polygonCollider.GetPoints() )
    {
        if ( !clipPlane.IsOnPlane( colliderTwoPoint ) )
        {
            continue;
        }

        const float bTangent = Vec2::Dot( colliderTwoPoint, tangent );

        if ( bTangent > bMax )
        {
            bMax = bTangent;
        }
        if ( bTangent < bMin )
        {
            bMin = bTangent;
        }
    }

    const Vec2 minEdge = bMin * tangent + clipPlane.normal * clipPlane.distance;
    const Vec2 maxEdge = bMax * tangent + clipPlane.normal * clipPlane.distance;

    return LineSeg2D( minEdge, maxEdge );
}

static LineSeg2D GetContactEdge( const PolygonCollider2D& polygonCollider, const LineSeg2D& referenceEdge, const Plane2D& clipPlane )
{
    const Vec2 referenceEdgeNormal = referenceEdge.GetNormal();
    const Vec2 referencePlaneTangent = clipPlane.GetTangent();
    const std::vector<Vec2>& colliderOnePoints = polygonCollider.GetPoints();

    float bMin = Vec2::Dot( referenceEdge.start, referencePlaneTangent );
    float bMax = Vec2::Dot( referenceEdge.end, referencePlaneTangent );

    for ( size_t colliderOneIndex = 0; colliderOneIndex < colliderOnePoints.size(); ++
          colliderOneIndex )
    {
        const Vec2& pointOne = colliderOnePoints.at( colliderOneIndex );
        Vec2 pointTwo;
        if ( colliderOneIndex < colliderOnePoints.size() - 1 )
        {
            pointTwo = colliderOnePoints.at( colliderOneIndex + 1 );
        }
        else
        {
            pointTwo = colliderOnePoints.at( 0 );
        }
        const LineSeg2D colliderOneSeg( pointTwo, pointOne );

        // If the planes are facing the same way skip them
        if ( Vec2::Dot( colliderOneSeg.GetNormal(), referenceEdgeNormal ) < 0 ) { continue; }

        LineSeg2D clipped;
        // If the clip was successful
        if ( colliderOneSeg.GetClippedTo( referenceEdge, clipped ) )
        {
            const Vec2 start = clipped.start;
            const Vec2 end = clipped.end;

            // Draw clipped line segments
            if( g_ShowGJKDebug && g_ShowGJKDebugLevel >= 2 )
            {
                DebugRenderer::AddWorldRay( LineSeg3D( LineSeg2D( end, start ) ), Rgba8::MAGENTA, .1f,
                                            .01f );
            }

            if ( !clipPlane.IsOnPositiveSide( start ) )
            {
                const float aTangent = Vec2::Dot( start, referencePlaneTangent );

                if ( aTangent < bMax )
                {
                    bMax = aTangent;
                }
                if ( aTangent > bMin )
                {
                    bMin = aTangent;
                }
            }
            if ( !clipPlane.IsOnPositiveSide( end ) )
            {
                const float aTangent = Vec2::Dot( end, referencePlaneTangent );

                if ( aTangent < bMax )
                {
                    bMax = aTangent;
                }
                if ( aTangent > bMin )
                {
                    bMin = aTangent;
                }
            }
        }
    }

    const Vec2 finalMin = bMin * referencePlaneTangent + clipPlane.normal * clipPlane.distance;
    const Vec2 finalMax = bMax * referencePlaneTangent + clipPlane.normal * clipPlane.distance;
    return  LineSeg2D( finalMin, finalMax );
}

static bool PolygonVsPolygonCollisionCallback( const Collider2D* colliderOne,
                                               const Collider2D* colliderTwo,
                                               OUT_PARAM Manifold2D& manifold )
{
    std::vector<Vec2> simplexPoints;
    static LineSeg2D simplexLines[ 3 ];
    const PolygonCollider2D* colliderOnePolygon = dynamic_cast<const PolygonCollider2D*>(colliderOne
    );
    const PolygonCollider2D* colliderTwoPolygon = dynamic_cast<const PolygonCollider2D*>(colliderTwo
    );

    // Start at the left
    simplexPoints.push_back(
        GetMinkowskiSupport( *colliderOnePolygon, *colliderTwoPolygon, Vec2::UNIT_WEST ) );
    // Get the point to the furthest right
    simplexPoints.push_back(
        GetMinkowskiSupport( *colliderOnePolygon, *colliderTwoPolygon, Vec2::UNIT_EAST ) );

    // Get third simplex point
    const LineSeg2D pointOneTwo( simplexPoints[ 0 ], simplexPoints[ 1 ] );
    const bool onPositiveSide = pointOneTwo.IsOnPositiveSide( Vec2::ZERO );
    const Vec2 pointThreeDirection = pointOneTwo.GetNormal() * (onPositiveSide ? 1.f : -1.f);

    simplexPoints.push_back(
        GetMinkowskiSupport( *colliderOnePolygon, *colliderTwoPolygon, pointThreeDirection ) );

    const bool isOriginContained = ExpandForOrigin( *colliderOnePolygon, *colliderTwoPolygon,
                                              simplexPoints );
    if( colliderOne->IsTrigger() || colliderTwo->IsTrigger() )
    {
        return  isOriginContained;
    }
    else if ( !isOriginContained ) { return false; }


    // // Debug Render minkowski space
    if( g_ShowGJKDebug && g_ShowGJKDebugLevel >= 1 )
    {
        for ( const Vec2& pointOne : colliderOnePolygon->GetPoints() )
        {
            for ( const Vec2& pointTwo : colliderTwoPolygon->GetPoints() )
            {
                DebugRenderer::AddWorldPoint( Vec3( pointOne - pointTwo ), .1f, Rgba8::YELLOW, .01f );
            }
        }
        
        LineSeg2D simplexLineOneTwo = LineSeg2D( simplexPoints[0], simplexPoints[1] );
        DebugRenderer::AddWorldRay( static_cast<LineSeg3D>(simplexLineOneTwo), Rgba8::BLACK, .25f, .01f,
                                    RENDER_ALWAYS );
        DebugRenderer::AddWorldRay( LineSeg3D( LineSeg2D( simplexLineOneTwo.GetCenter(),
                                                          simplexLineOneTwo.GetCenter() +
                                                          simplexLineOneTwo.GetNormal() ) ),
                                    Rgba8::RED, .1f, .01f,
                                    RENDER_ALWAYS );
        
        LineSeg2D simplexLineTwoThree = LineSeg2D( simplexPoints[1], simplexPoints[2] );
        DebugRenderer::AddWorldRay( static_cast<LineSeg3D>(simplexLineTwoThree), Rgba8::BLACK, .25f,
                                    .01f,
                                    RENDER_ALWAYS );
        DebugRenderer::AddWorldRay( LineSeg3D( LineSeg2D( simplexLineTwoThree.GetCenter(),
                                                          simplexLineTwoThree.GetCenter() +
                                                          simplexLineTwoThree.GetNormal() ) ),
                                    Rgba8::RED, .1f, .01f,
                                    RENDER_ALWAYS );
        
        LineSeg2D simplexLineThreeOne = LineSeg2D( simplexPoints[2], simplexPoints[0] );
        DebugRenderer::AddWorldRay( static_cast<LineSeg3D>(simplexLineThreeOne), Rgba8::BLACK, .25f,
                                    .01f,
                                    RENDER_ALWAYS );
        DebugRenderer::AddWorldRay( LineSeg3D( LineSeg2D( simplexLineThreeOne.GetCenter(),
                                                          simplexLineThreeOne.GetCenter() +
                                                          simplexLineThreeOne.GetNormal() ) ),
                                    Rgba8::RED, .1f, .01f,
                                    RENDER_ALWAYS );
    }
    
    Vec2 penetrationVector = CalculatePenetration( *colliderOnePolygon, *colliderTwoPolygon,
                                                   simplexPoints );
    const float penetration = penetrationVector.NormalizeAndGetPreviousLength();
    manifold.normal = penetrationVector;
    manifold.penetration = penetration;

    if (g_ShowGJKDebug && g_ShowGJKDebugLevel >= 1 )
    {
        DebugRenderer::AddWorldArrow( LineSeg3D( LineSeg2D( Vec2::ZERO, penetrationVector * penetrationVector ) ), Rgba8::CYAN, .01f );
    }

    const Plane2D clipPlane( manifold.normal,
                             colliderTwoPolygon->GetSupportPoint( manifold.normal ) );

    const LineSeg2D referenceEdge = GetReferenceEdge( *colliderTwoPolygon, clipPlane);
    // If there is only one contact point
    if ( referenceEdge.start == referenceEdge.end )
    {
        manifold.contactEdge = referenceEdge;
    }
    else
    {
        const LineSeg2D contactEdge = GetContactEdge( *colliderOnePolygon, referenceEdge, clipPlane );
        manifold.contactEdge = contactEdge;
    }

    if( g_ShowGJKDebug && g_ShowGJKDebugLevel >= 0 )
    {
        DebugRenderer::AddWorldPoint( Vec3( manifold.contactEdge.start ), .5f, Rgba8::RED, .01f,
                                      RENDER_ALWAYS );
        DebugRenderer::AddWorldPoint( Vec3( manifold.contactEdge.end ), .35f, Rgba8::GREEN, .01f,
                                      RENDER_ALWAYS );
    }
    
    return true;
}

static CollisionCheckCallback g_CollisionCallbacks[ NUM_COLLIDER_TYPES * NUM_COLLIDER_TYPES ] = {
    // Matrix       Disc,                           Polygon,
    /*   Disc */ DiscVsDiscCollisionCallback, nullptr,
    /* Polygon */ DiscVsPolygonCollisionCallback, PolygonVsPolygonCollisionCallback,
};


float Collider2D::GetRestitutionAgainst( const Collider2D& other ) const
{
    return m_Material.restitution * other.m_Material.restitution;
}

float Collider2D::GetFrictionAgainst( const Collider2D& other ) const
{
    return m_Material.friction * other.m_Material.friction;
}

Collider2D::Collider2D( Physics2D* physicsWorld )
    : m_PhysicsSystem( physicsWorld )
{
}

Collider2D::~Collider2D()
{
    m_PhysicsSystem = nullptr;
    m_Rigidbody = nullptr;
}


void Collider2D::UpdateWorldShape()
{
    if ( m_Rigidbody != nullptr )
    {
        m_WorldPosition = m_Rigidbody->m_WorldPosition;
    }
}

bool Collider2D::Intersects( Collider2D& other, OUT_PARAM Collision2D& collision )
{
    // Do layers collide
    if( !m_PhysicsSystem->DoLayersInteract( m_CollisionLayer, other.m_CollisionLayer ) )
    {
        return false;
    }

    const Disc myDisc = GetWorldBounds();
    const Disc otherDisc = other.GetWorldBounds();

    if ( !Disc::Intersects( myDisc, otherDisc ) )
    {
        return false;
    }

    return FineRangeIntersection( other, collision );
}

bool Collider2D::FineRangeIntersection( Collider2D& other, OUT_PARAM Collision2D& collision )
{
    collision.self = this;
    collision.other = &other;

    if ( m_Type <= other.m_Type )
    {
        const int collisionCallbackIndex = other.m_Type * NUM_COLLIDER_TYPES + m_Type;
        const CollisionCheckCallback cb = g_CollisionCallbacks[ collisionCallbackIndex ];

        return cb( this, &other, collision.manifold );
    }
    const int collisionCallbackIndex = m_Type * NUM_COLLIDER_TYPES + other.m_Type;
    const CollisionCheckCallback cb = g_CollisionCallbacks[ collisionCallbackIndex ];

    const bool intersects = cb( &other, this, collision.manifold );
    collision.manifold.normal = -collision.manifold.normal;
    return intersects;
}
