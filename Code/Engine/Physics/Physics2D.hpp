#pragma once

#include <vector>
#include "Engine/Core/Math/Primatives/IntVec2.hpp"

#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Time/Timer.hpp"

#include "Collider/Collision2D.hpp"

//-----------------------------------------------------------------------------
//Engine Predefines
struct Disc;
class Rigidbody2D;
class Collider2D;
class DiscCollider2D;
class PolygonCollider2D;

struct CollisionIdentifier
{
    IntVec2 index = IntVec2( -1, 0 );
    unsigned int stepIndex = ~0u;

    Collision2D collision;
};

class Physics2D
{
public:
    Clock* m_PhysicsClock = nullptr;
    Physics2D( Clock* physicsClockParent);

    void BeginFrame();
    void Update();
    void EndFrame();

    Vec2 GetSceneGravity() const { return m_Gravity; }
    void SetSceneGravity( const Vec2& newGravity );

    double GetFixedTimeDelta() const;
    void SetFixedTimedDelta( double frameTimeSeconds);

    Rigidbody2D* CreateRigidbody( const Vec2& worldPosition = Vec2::ZERO, Collider2D* collider = nullptr );
    void DestroyRigidbody( Rigidbody2D* rb );

    DiscCollider2D* CreateDiscCollider( const Disc& localDisc );
    PolygonCollider2D* CreatePolygonCollider( const std::vector<Vec2>& points, bool isCloud = true );
    void DestroyCollider( Collider2D* collider );

    void EnableLayerInteraction( unsigned int layer1, unsigned int layer2 );
    void DisableLayerInteraction( unsigned int layer1, unsigned int layer2 );
    bool DoLayersInteract( unsigned int layer1, unsigned int layer2 );

    const std::vector<Collision2D>& DebugGetLastFrameCollisions() const { return m_LastFrameCollisions; }

private:
    Vec2 m_Gravity = Vec2(0.f, -9.8f);

    std::vector<Rigidbody2D*> m_RigidBodies;
    std::vector<Collider2D*> m_Colliders;
    std::vector<Collision2D> m_StepCollisions;
    std::vector<Collision2D> m_LastFrameCollisions;

    unsigned int m_StepIndex = 0;
    std::vector<CollisionIdentifier> m_LastStepCollisions;

    // Using unsigned int flags for layers
    unsigned int m_CollisionMatrix[ 32 ];

    void SimulateStep();

    void BeginSimulationStep();
    void ApplyGlobalForces();
    void MoveObjects( float deltaSeconds );
    void DetectCollisions();
    void ResolveCollisions();
    void ResolveIndividualCollision( Collision2D& collision );
    void PushOutOf( const Collision2D& collision );
    void ApplyCollisionImpulses( const Collision2D& collision );
    void EndSimulationStep();
    void ResetObjects();

    void DetermineMidstepCallbacks( size_t index1, size_t index2, const Collision2D* collision);
    void DetermineEndstepCallbacks();

    void DestroyRequestedRigidBodies();
    void DestroyRequestedCollider2Ds();
};