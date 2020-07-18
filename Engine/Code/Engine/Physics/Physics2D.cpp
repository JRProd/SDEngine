#include "Physics2D.hpp"


#include "Engine/Console/Console.hpp"
#include "Engine/Event/EventSystem.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider/Collider2D.hpp"
#include "Engine/Physics/Collider/Collision2D.hpp"
#include "Engine/Physics/Collider/DiscCollider2D.hpp"
#include "Engine/Physics/Collider/PolygonCollider2D.hpp"

#include <cmath>
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Renderer/DebugRenderer.hpp"

static bool g_CalculateCollisionResponse = true;
static Timer* g_FixedDeltaTime = nullptr;

#if !defined(ENGINE_DISABLE_CONSOLE)
static bool CommandSetCollisions( EventArgs* args )
{
    const int numArgs = args->GetValue( "numArgs", 0 );

    if ( numArgs == 0 )
    {
        g_Console->InvalidArgument( "SetCollisions",
                                    "Set collisions command requires a bool after the command" );
        return false;
    }
    const bool collision = args->GetValue( "Arg1", false);
    g_CalculateCollisionResponse = collision;
    return true;
}

static bool CommandToggleCollisions( EventArgs* args )
{
    UNUSED( args );

    g_CalculateCollisionResponse = !g_CalculateCollisionResponse;
    return true;
}

bool CommandSetPhysicsUpdate( EventArgs* args )
{
    const int numArgs = args->GetValue( "numArgs", 0 );

    if ( numArgs == 0 )
    {
        g_Console->InvalidArgument( "SetPhysicsUpdate",
                                    "Set physics update command requires a hrz after the command" );
        return false;
    }
    const float hrz = 1.f / args->GetValue( "Arg1", 1.f / 120.f );
    g_FixedDeltaTime->Set( hrz );
    return true;
}
#endif // !defined(ENGINE_DISABLE_CONSOLE)

Physics2D::Physics2D( Clock* physicsClockParent )
{
    m_PhysicsClock = new Clock( physicsClockParent );
    g_FixedDeltaTime = new Timer();
    g_FixedDeltaTime->Set( m_PhysicsClock, 1.f / 120.f );

    memset( m_CollisionMatrix, ~0, sizeof( m_CollisionMatrix ) );

#if !defined(ENGINE_DISABLE_CONSOLE)
    Command setCollision;
    setCollision.commandName = "Set_Collisions";
    setCollision.arguments.push_back( new TypedArgument<bool>("", false ) );
    setCollision.description = "Set the physics world collisions";
    Console::RegisterCommand( setCollision, CommandSetCollisions );

    Console::RegisterCommand( "ToggleCollisions", "Toggles the collisions for this physics world",
                              CommandToggleCollisions );

    Command setPhysicsUpdate;
    setPhysicsUpdate.commandName = "Set_Physics_Update";
    setPhysicsUpdate.arguments.push_back( new TypedArgument<float>("hrz", false ) );
    setPhysicsUpdate.description = "Sets the hertz of the physics update";
    Console::RegisterCommand( setPhysicsUpdate, CommandSetPhysicsUpdate );

    Command debugGJK;
    debugGJK.commandName = "Debug_Collision_GJK";
    debugGJK.arguments.push_back( new TypedArgument<bool> ("toggle", true, false ) );
    debugGJK.arguments.push_back( new TypedArgument<std::string>("verbosity", true, false ) );
    debugGJK.description = "Toggles debug rendering of the GJK algorithm";
    Console::RegisterCommand( debugGJK, &CommandDebugCollisionGJK );
#endif // !defined(ENGINE_DISABLE_CONSOLE)
}

void Physics2D::BeginFrame()
{
    std::vector<Collider2D*>::iterator currCollider;
    for ( currCollider = m_Colliders.begin(); currCollider != m_Colliders.end(); ++currCollider )
    {
        Collider2D* collider = *currCollider;
        if ( collider != nullptr )
        {
            collider->UpdateWorldShape();
        }
    }
}

void Physics2D::Update()
{
    while ( g_FixedDeltaTime->CheckAndDecrement() )
    {
        m_StepIndex += 1;
        SimulateStep();
    }
}

void Physics2D::SimulateStep()
{
    BeginSimulationStep();
    ApplyGlobalForces();
    MoveObjects( static_cast<float>(g_FixedDeltaTime->GetIncrement()) );
    EndSimulationStep();

    DetectCollisions();
    ResolveCollisions();

    DetermineEndstepCallbacks();
}

void Physics2D::EndFrame()
{
    ResetObjects();
    m_LastFrameCollisions.clear();

    DestroyRequestedCollider2Ds();
    DestroyRequestedRigidBodies();
}

void Physics2D::SetSceneGravity( const Vec2& newGravity )
{
    m_Gravity = newGravity;
}

double Physics2D::GetFixedTimeDelta() const
{
    return g_FixedDeltaTime->GetIncrement();
}

void Physics2D::SetFixedTimedDelta( double frameTimeSeconds )
{
    g_FixedDeltaTime->Set( frameTimeSeconds );
}

Rigidbody2D* Physics2D::CreateRigidbody( const Vec2& worldPosition, Collider2D* collider )
{
    Rigidbody2D* rigidBody = new Rigidbody2D( this, worldPosition, collider );
    m_RigidBodies.push_back( rigidBody );
    return rigidBody;
}

void Physics2D::DestroyRigidbody( Rigidbody2D* rb )
{
    if ( rb == nullptr ) { return; }

    rb->Destroy();
}

DiscCollider2D* Physics2D::CreateDiscCollider( const Disc& localDisc )
{
    DiscCollider2D* collider = new DiscCollider2D( this, localDisc );
    m_Colliders.push_back( collider );
    return collider;
}

PolygonCollider2D* Physics2D::CreatePolygonCollider( const std::vector<Vec2>& points, bool isCloud )
{
    PolygonCollider2D* collider = new PolygonCollider2D(this, points, isCloud );
    m_Colliders.push_back( collider );
    return collider;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
    if ( collider == nullptr ) { return; }

    for( const CollisionIdentifier& collisionId : m_LastStepCollisions )
    {
        if( collisionId.index != IntVec2(-1, 0) )
        {
            if( collisionId.collision.self == collider || collisionId.collision.other == collider )
            {
                Collider2D* collider1 = collisionId.collision.self;
                Collider2D* collider2 = collisionId.collision.other;

                if ( collider1->IsTrigger() )
                {
                    collider1->OnTriggerEnd( collisionId.collision );
                }
                else if ( collider2->IsTrigger() )
                {
                    collider2->OnTriggerEnd( collisionId.collision.GetInverted() );
                }
                else
                {
                    collider1->OnOverlapEnd( collisionId.collision );
                    collider2->OnOverlapEnd( collisionId.collision.GetInverted() );
                }
            }
        }
    }

    collider->m_DestroyRequested = true;
}

void Physics2D::EnableLayerInteraction( const unsigned int layer1, const unsigned int layer2 )
{
    m_CollisionMatrix[ layer1 ] |= (1 << layer2);
    m_CollisionMatrix[ layer2 ] |= (1 << layer1);
}

void Physics2D::DisableLayerInteraction( const unsigned int layer1, const unsigned int layer2 )
{
    const unsigned int mask1 = ~(1 << layer2);
    const unsigned int mask2 = ~(1 << layer1);

    m_CollisionMatrix[ layer1 ] &= mask1;
    m_CollisionMatrix[ layer2 ] &= mask2;
}

bool Physics2D::DoLayersInteract( const unsigned int layer1, const unsigned int layer2 )
{
    return (m_CollisionMatrix[ layer1 ] & (1 << layer2)) != 0;
}

void Physics2D::BeginSimulationStep()
{
    std::vector<Rigidbody2D*>::iterator currRigidbody;
    for ( currRigidbody = m_RigidBodies.begin(); currRigidbody != m_RigidBodies.end(); ++
          currRigidbody )
    {
        Rigidbody2D* rigidbody = *currRigidbody;
        if ( rigidbody != nullptr )
        {
            rigidbody->m_StartFramePosition = rigidbody->m_WorldPosition;
        }
    }
}

void Physics2D::ApplyGlobalForces()
{
    for ( Rigidbody2D* rigidBody : m_RigidBodies )
    {
        if ( rigidBody == nullptr ) { continue; }

        rigidBody->ApplyDrag();
        rigidBody->AddForce( m_Gravity * rigidBody->GetMass() );
    }
}

void Physics2D::MoveObjects( float deltaSeconds )
{
    for ( Rigidbody2D* rigidBody : m_RigidBodies )
    {
        if ( rigidBody == nullptr ) { continue; }

        rigidBody->Update( deltaSeconds );
        rigidBody->m_Collider->UpdateWorldShape();
    }
}

void Physics2D::DetectCollisions()
{
    const size_t numberOfColliders = m_Colliders.size();
    for ( size_t colliderOneIndex = 0; colliderOneIndex < numberOfColliders; ++colliderOneIndex )
    {
        Collider2D*& colliderOne = m_Colliders.at( colliderOneIndex );
        if ( colliderOne == nullptr ) { continue; }

        for ( size_t colliderTwoIndex = colliderOneIndex + 1; colliderTwoIndex < numberOfColliders;
              ++colliderTwoIndex )
        {
            Collider2D*& colliderTwo = m_Colliders.at( colliderTwoIndex );
            if ( colliderTwo == nullptr ) { continue; }
            if ( colliderOne->m_Rigidbody->GetSimulationMode() == SimulationMode::STATIONARY &&
                colliderTwo->m_Rigidbody->GetSimulationMode() == SimulationMode::STATIONARY )
            {
                continue;
            }

            if( colliderOne->m_Rigidbody->IsActive() && colliderTwo->m_Rigidbody->IsActive() )
            {
                Collision2D collision;
                if ( colliderOne->Intersects( *colliderTwo, collision ) )
                {
                    m_StepCollisions.push_back( collision );
                    m_LastFrameCollisions.push_back( collision );

                    DetermineMidstepCallbacks( colliderOneIndex, colliderTwoIndex, &collision );
                }
            }
        }
    }
}

void Physics2D::ResolveCollisions()
{
    for ( Collision2D collision : m_StepCollisions )
    {
        if( collision.self->IsTrigger() || collision.other->IsTrigger() ) { continue; }

        ResolveIndividualCollision( collision );
    }
}

void Physics2D::ResolveIndividualCollision( Collision2D& collision )
{
    // Move objects out of each other
    PushOutOf( collision );

    // Calculate Impulse
    ApplyCollisionImpulses( collision );
}

void Physics2D::PushOutOf( const Collision2D& collision )
{
    const SimulationMode selfMode = collision.self->m_Rigidbody->GetSimulationMode();
    const SimulationMode otherMode = collision.other->m_Rigidbody->GetSimulationMode();

    float selfMass = collision.self->m_Rigidbody->GetMass();
    float otherMass = collision.other->m_Rigidbody->GetMass();

    bool isOtherInfinity = false;
    if ( selfMode == SimulationMode::DYNAMIC && otherMode != SimulationMode::DYNAMIC ||
        selfMode == SimulationMode::KINEMATIC && otherMode == SimulationMode::STATIONARY )
    {
        otherMass = INFINITY;
        isOtherInfinity = true;
    }
    else if ( otherMode == SimulationMode::DYNAMIC && selfMode != SimulationMode::DYNAMIC ||
        otherMode == SimulationMode::KINEMATIC && selfMode == SimulationMode::STATIONARY )
    {
        selfMass = INFINITY;
    }

    const Vec2 displacement = collision.manifold.normal * collision.manifold.penetration;
    const float combinedMass = selfMass + otherMass;

    float pushSelf = otherMass / combinedMass;
    if ( isOtherInfinity )
    {
        pushSelf = 1.f;
    }
    const float pushOther = 1.f - pushSelf;


    if ( !g_CalculateCollisionResponse ) { return; }

    collision.self->m_Rigidbody->m_WorldPosition += pushSelf * displacement;
    collision.other->m_Rigidbody->m_WorldPosition -= pushOther * displacement;
}

void Physics2D::ApplyCollisionImpulses( const Collision2D& collision )
{
    const SimulationMode selfMode = collision.self->m_Rigidbody->GetSimulationMode();
    const SimulationMode otherMode = collision.other->m_Rigidbody->GetSimulationMode();

    float selfInverseMass = 1.f / collision.self->m_Rigidbody->GetMass();
    float selfInverseMoment = 1.f / collision.self->m_Rigidbody->GetMoment();
    float otherInverseMass = 1.f / collision.other->m_Rigidbody->GetMass();
    float otherInverseMoment = 1.f / collision.other->m_Rigidbody->GetMoment();

    const Vec2 edgePointOnSelf = collision.manifold.contactEdge.GetNearestPointOnLineSeg2D( collision.self->m_WorldPosition );
    const Vec2 edgePointOnOther = collision.manifold.contactEdge.GetNearestPointOnLineSeg2D( collision.other->m_WorldPosition );

    const Vec2 radiusSelfContactT = (edgePointOnSelf - collision.self->m_Rigidbody->m_WorldPosition).GetRotated90Degrees();
    const Vec2 radiusOtherContactT = (edgePointOnOther - collision.other->m_Rigidbody->m_WorldPosition).GetRotated90Degrees();

    if ( selfMode == SimulationMode::DYNAMIC && otherMode != SimulationMode::DYNAMIC ||
        selfMode == SimulationMode::KINEMATIC && otherMode == SimulationMode::STATIONARY )
    {
        otherInverseMass = 0.f;
        otherInverseMoment = 0.f;
    }
    else if ( otherMode == SimulationMode::DYNAMIC && selfMode != SimulationMode::DYNAMIC ||
        otherMode == SimulationMode::KINEMATIC && selfMode == SimulationMode::STATIONARY )
    {
        selfInverseMass = 0.f;
        selfInverseMoment = 0.f;
    }

    // Calculate normal impulse
    const float restitutionEffect = 1.f + collision.self->GetRestitutionAgainst( *collision.other );
    Vec2 velocityDifference = collision.other->m_Rigidbody->GetImpactVelocity( edgePointOnOther )
        - collision.self->m_Rigidbody->GetImpactVelocity( edgePointOnSelf );
    const float topNormal = restitutionEffect * Vec2::Dot( velocityDifference, collision.manifold.normal );

    const float combinedInverseMass = selfInverseMass + otherInverseMass;
    const float selfCrossNormal = Vec2::Dot( radiusSelfContactT, collision.manifold.normal );
    const float selfMomentEffectNormal = selfCrossNormal * selfCrossNormal * selfInverseMoment;
    const float otherCrossNormal = Vec2::Dot( radiusOtherContactT, collision.manifold.normal );
    const float otherMomentEffectNormal = otherCrossNormal * otherCrossNormal * otherInverseMoment;

    const float bottomNormal = combinedInverseMass + selfMomentEffectNormal + otherMomentEffectNormal;
    float normalImpulse = topNormal / bottomNormal;
    normalImpulse = Maxf( normalImpulse, 0.f );


    if ( !g_CalculateCollisionResponse ) { return; }

    // Apply Normal Impulse
    collision.self->m_Rigidbody->ApplyImpulse( collision.manifold.normal * normalImpulse, edgePointOnSelf );
    collision.other->m_Rigidbody->ApplyImpulse( collision.manifold.normal * -normalImpulse, edgePointOnOther );

    // Calculate tangent impulse after applying normal impulse
    velocityDifference = collision.other->m_Rigidbody->GetImpactVelocity( edgePointOnOther )
        - collision.self->m_Rigidbody->GetImpactVelocity( edgePointOnSelf );

    const Vec2 tangent = collision.manifold.normal.GetRotated90Degrees();
    const float friction = collision.self->GetFrictionAgainst( *collision.other );
    const float topTangent = restitutionEffect * Vec2::Dot( velocityDifference, tangent );

    const float selfCrossTangent = Vec2::Dot( radiusSelfContactT, tangent );
    const float selfMomentEffectTangent = selfCrossTangent * selfCrossTangent * selfInverseMoment;
    const float otherCrossTangent = Vec2::Dot( radiusOtherContactT, tangent );
    const float otherMomentEffectTangent = otherCrossTangent * otherCrossTangent * otherInverseMoment;

    const float bottomTangent = combinedInverseMass + selfMomentEffectTangent + otherMomentEffectTangent;
    float tangentImpulse = topTangent / bottomTangent;
    if ( abs( tangentImpulse ) > normalImpulse * friction )
    {
        tangentImpulse = Signf( tangentImpulse ) * normalImpulse * friction;
    }

    // float tangentialImpulse = massPortion * restitutionEffect * frictionDirection;
    // Apply Friction Impulse
    collision.self->m_Rigidbody->ApplyImpulse( tangent * tangentImpulse, edgePointOnSelf );
    collision.other->m_Rigidbody->ApplyImpulse( tangent * -tangentImpulse, edgePointOnOther );
}

void Physics2D::EndSimulationStep()
{
    for ( Rigidbody2D* rigidBody : m_RigidBodies )
    {
        if ( rigidBody == nullptr ) { continue; }

        rigidBody->UpdateVerletVelocity( static_cast<float>(g_FixedDeltaTime->GetIncrement()) );
    }

    m_StepCollisions.clear();
}

void Physics2D::ResetObjects()
{
    for ( Rigidbody2D* rigidBody : m_RigidBodies )
    {
        if ( rigidBody == nullptr ) { continue; }

        rigidBody->ResetForce();
    }
}

void Physics2D::DetermineMidstepCallbacks( const size_t index1, const size_t index2, const Collision2D* collision)
{
    const IntVec2 index = IntVec2( static_cast<int>(Minu( index1, index2 )), static_cast<int>(Maxu( index1, index2 ) ) );

    Collider2D* collider1 = collision->self;
    Collider2D* collider2 = collision->other;

    for( CollisionIdentifier& collisionId : m_LastStepCollisions )
    {
        if( collisionId.index == index )
        {
            if ( collisionId.stepIndex + 1 == m_StepIndex )
            {
                if( collider1->IsTrigger() ) {
                    collider1->OnTrigger( *collision );
                }
                else if (collider2->IsTrigger() )
                {
                    collider2->OnTrigger( collision->GetInverted() );
                }
                else
                {
                    collider1->OnOverlap( *collision );
                    collider2->OnOverlap( collision->GetInverted() );
                }

                collisionId.collision = *collision;
                collisionId.stepIndex = m_StepIndex;
                return;
            }
        }
    }

    CollisionIdentifier collisionId;
    collisionId.index = index;
    collisionId.stepIndex = m_StepIndex;
    collisionId.collision = *collision;
    m_LastStepCollisions.push_back( collisionId );

    if ( collider1->IsTrigger() )
    {
        collider1->OnTriggerBegin( *collision );
    }
    else if ( collider2->IsTrigger() )
    {
        collider2->OnTriggerBegin( collision->GetInverted() );
    }
    else
    {
        collider1->OnOverlapBegin( *collision );
        collider2->OnOverlapBegin( collision->GetInverted() );
    }
}

void Physics2D::DetermineEndstepCallbacks()
{
    size_t listSize = m_LastStepCollisions.size();
    for ( size_t index = 0; index < listSize; ++index )
    {
        CollisionIdentifier& collisionId = m_LastStepCollisions.at( index );
        // Checks to see if should remove current index
        if ( collisionId.index == IntVec2( -1, 0 ) )
        {
            // Update to the last one in the 
            collisionId = m_LastStepCollisions.at( m_LastStepCollisions.size() - 1 );
            m_LastStepCollisions.pop_back();
            listSize -= 1;

            if( listSize <= 0 )
            {
                return;
            }
        }

        // Checks to see if collision is old
        if ( collisionId.stepIndex != m_StepIndex )
        {
            Collision2D collision = collisionId.collision;
            Collider2D* collider1 = collision.self;
            Collider2D* collider2 = collision.other;

            if ( collider1->IsTrigger() )
            {
                collider1->OnTriggerEnd( collision );
            }
            else if ( collider2->IsTrigger() )
            {
                collider2->OnTriggerEnd( collision.GetInverted() );
            }
            else
            {                     
                collider1->OnOverlapEnd( collision );
                collider2->OnOverlapEnd( collision.GetInverted() );
            }


            collisionId.index = IntVec2(-1, 0);
        }
    }
}

void Physics2D::DestroyRequestedRigidBodies()
{
    std::vector<Rigidbody2D*>::iterator currPosition;
    for ( currPosition = m_RigidBodies.begin(); currPosition != m_RigidBodies.end(); ++currPosition
    )
    {
        Rigidbody2D*& currRigidBody = *currPosition;
        if ( currRigidBody != nullptr )
        {
            if ( currRigidBody->m_DestroyRequested )
            {
                delete*currPosition;
                *currPosition = nullptr;
            }
        }
    }
}

void Physics2D::DestroyRequestedCollider2Ds()
{
    std::vector<Collider2D*>::iterator currPosition;
    for ( currPosition = m_Colliders.begin(); currPosition != m_Colliders.end(); ++currPosition )
    {
        Collider2D*& currCollider = *currPosition;
        if ( currCollider != nullptr )
        {
            if ( currCollider->m_DestroyRequested )
            {
                delete *currPosition;
                *currPosition = nullptr;
            }
        }
    }
}
