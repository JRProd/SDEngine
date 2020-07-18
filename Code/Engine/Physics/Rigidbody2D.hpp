#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"

//-----------------------------------------------------------------------------
// Engine Predefines
class Physics2D;
class Collider2D;

constexpr float STARTING_MASS = 1.f;

enum class SimulationMode
{
    STATIONARY,
    KINEMATIC,
    DYNAMIC
};

class Rigidbody2D
{
    friend class Physics2D;

public:
    Vec2 m_WorldPosition;

    explicit Rigidbody2D( Physics2D* physicsWorld, const Vec2& worldPosition, float mass = STARTING_MASS );
    Rigidbody2D( Physics2D* physicsWorld, const Vec2& worldPosition, Collider2D* collider, float mass = STARTING_MASS );

    SimulationMode GetSimulationMode() const { return m_SimulationMode; }
    void SetSimulationMode( const SimulationMode newMode ) { m_SimulationMode = newMode; }

    void AddForce( const Vec2& force );
    void AddTorque( float torque );
    void ApplyImpulse( const Vec2& impulse );
    void ApplyImpulse( const Vec2& impulse, const Vec2& point );
    void ApplyDrag();

    float GetMass() const { return m_Mass; }
    float GetInverseMass() const { return m_InverseMass; }
    float GetAngleDegrees() const;
    float GetAngleRadians() const { return m_AngleInRadians; }
    float GetMoment() const { return m_Moment; }
    float GetLinearDrag() const { return m_LinearDrag; }
    float GetAngularDrag() const { return m_AngularDrag; }
    Vec2 GetVelocity() const { return m_Velocity; }
    Vec2 GetVerletVelocity() const { return m_VerletVelocity; }
    Vec2 GetImpactVelocity( const Vec2& impactPoint ) const;
    float GetAngularVelocity() const { return m_AngularVelocity; }

    void AddAngleDegrees( float deltaDegrees );
    void AddAngleRadians( float deltaRadians );
    void SetAngleDegrees( float angleDegrees );
    void SetAngleRadians( float angleRadians );
    void AddAngularVelocity( float deltaDegrees );
    void SetAngularVelocity( float angleDegrees );
    void SetMass( float mass );
    void SetLinearDrag( const float drag ) { m_LinearDrag = drag; }
    void SetAngularDrag(const float drag ) { m_AngularDrag = drag; }
    void SetVelocity( const Vec2& velocity ) { m_Velocity = velocity; }

    void SetUserData( void* data ) { m_UserData = data; }
    void* GetUserData() const { return m_UserData; }

    void Disable() { m_Active = false; }
    void Enable() { m_Active = true; }
    void SetActive( bool newActive ) { m_Active = newActive; }
    bool IsActive() const { return m_Active; }

    Collider2D* GetCollider() const { return m_Collider; }

    void Destroy();

private:
    Physics2D* m_PhysicsSystem = nullptr;
    Collider2D* m_Collider = nullptr;

    void* m_UserData = nullptr;

    // Physics Attributes
    SimulationMode m_SimulationMode = SimulationMode::DYNAMIC;
    bool m_Active = true;
    float m_Mass = STARTING_MASS;
    float m_InverseMass = 1.f / STARTING_MASS;
    float m_LinearDrag = 1.f;
    float m_AngularDrag = 0.1f;

    Vec2 m_StartFramePosition = Vec2::ZERO;
    float m_AngleInRadians = 0.f;
    Vec2 m_Velocity = Vec2::ZERO;
    float m_AngularVelocity = 0.f;
    Vec2 m_VerletVelocity = Vec2::ZERO;
    Vec2 m_LastFrameForce = Vec2::ZERO;
    float m_LastFrameTorque = 0.f;
    float m_Moment = -1.f;

    bool m_DestroyRequested = false;
    ~Rigidbody2D();

    void Update( float deltaSeconds );
    void UpdateVerletVelocity( float deltaSeconds );
    void ResetForce();
};
