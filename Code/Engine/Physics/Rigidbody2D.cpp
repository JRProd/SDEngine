#include "Rigidbody2D.hpp"

#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/Collider/Collider2D.hpp"


Rigidbody2D::Rigidbody2D( Physics2D* physicsWorld, const Vec2& worldPosition, float mass )
    : m_WorldPosition( worldPosition )
        , m_PhysicsSystem(physicsWorld)
{
    SetMass( mass );
}

Rigidbody2D::Rigidbody2D( Physics2D* physicsWorld, const Vec2& worldPosition, Collider2D* collider, float mass )
    : m_WorldPosition( worldPosition )
        , m_PhysicsSystem( physicsWorld )
        , m_Collider( collider )
{
    m_Collider->m_Rigidbody = this;
    SetMass( mass );
}
void Rigidbody2D::AddForce( const Vec2& force )
{
    m_LastFrameForce += force;
}

void Rigidbody2D::AddTorque( const float torque )
{
    m_LastFrameTorque += torque;
}

void Rigidbody2D::ApplyImpulse( const Vec2& impulse )
{
    if ( m_SimulationMode == SimulationMode::DYNAMIC )
    {
        m_Velocity += impulse * m_InverseMass;
    }
}

void Rigidbody2D::ApplyImpulse( const Vec2& impulse, const Vec2& point )
{
    if( m_SimulationMode == SimulationMode::DYNAMIC )
    {
        m_Velocity += impulse * m_InverseMass;

        const Vec2 tangent = (point - m_WorldPosition).GetRotated90Degrees();
        const float rotationalImpulse = Vec2::Dot( impulse, tangent );
        m_AngularVelocity += rotationalImpulse / m_Moment;
    }
}

void Rigidbody2D::ApplyDrag()
{
    AddForce( -GetVelocity() * m_LinearDrag );
    AddTorque( -GetAngularVelocity() * m_AngularDrag );
}

Vec2 Rigidbody2D::GetImpactVelocity( const Vec2& impactPoint ) const
{
    const Vec2 radiusTangent = (impactPoint - m_WorldPosition).GetRotated90Degrees();

    return GetVelocity() + (m_AngularVelocity * radiusTangent);
}

void Rigidbody2D::AddAngleDegrees( const float deltaDegrees )
{
    AddAngleRadians( ConvertDegreesToRadians( deltaDegrees ) );
}

void Rigidbody2D::AddAngleRadians( const float deltaRadians )
{
    SetAngleRadians( m_AngleInRadians + deltaRadians );
}

void Rigidbody2D::SetAngleDegrees( const float angleDegrees )
{
    SetAngleRadians( ConvertDegreesToRadians( angleDegrees ) );
}

void Rigidbody2D::SetAngleRadians( float angleRadians )
{
    m_AngleInRadians = GetAngleNegPiToPi( angleRadians );
}

void Rigidbody2D::AddAngularVelocity( const float deltaDegrees )
{
    m_AngularVelocity += ConvertDegreesToRadians( deltaDegrees );
}

void Rigidbody2D::SetAngularVelocity( const float angleDegrees )
{
    m_AngularVelocity = ConvertDegreesToRadians( angleDegrees );
}

void Rigidbody2D::SetMass( const float mass )
{
    if ( m_Moment == -1.f && m_Collider != nullptr )
    {
        m_Moment = m_Collider->CalculateMoment( mass );
    }
    else
    {
        // Moment of Inertia is related to the mass. Don't need to recalculate if mass is changed
        m_Moment *= mass / m_Mass;
    }
    m_Mass = mass;
    m_InverseMass = 1.f / mass;
}

float Rigidbody2D::GetAngleDegrees() const
{
    return ConvertRadiansToDegrees( m_AngleInRadians );
}

void Rigidbody2D::Destroy()
{
    m_DestroyRequested = true;
    m_PhysicsSystem->DestroyCollider( m_Collider );
    m_Collider = nullptr;
    m_PhysicsSystem = nullptr;
}

Rigidbody2D::~Rigidbody2D()
{
    m_PhysicsSystem->DestroyCollider( m_Collider );
    m_Collider = nullptr;
    m_PhysicsSystem = nullptr;
}

void Rigidbody2D::Update( float deltaSeconds )
{
    if ( !m_Active ) { return; }
    if ( m_SimulationMode == SimulationMode::STATIONARY ) { return; }

    if ( m_SimulationMode == SimulationMode::DYNAMIC )
    {
        const Vec2 acceleration = m_LastFrameForce / m_Mass;
        m_Velocity += acceleration * deltaSeconds;
        const float angularAcceleration = m_LastFrameTorque / m_Moment; 
        m_AngularVelocity += angularAcceleration * deltaSeconds;
    }

    m_WorldPosition += m_Velocity * deltaSeconds;
    SetAngleRadians( m_AngleInRadians + m_AngularVelocity * deltaSeconds );
}

void Rigidbody2D::UpdateVerletVelocity( float deltaSeconds )
{
    m_VerletVelocity = (m_WorldPosition - m_StartFramePosition) / deltaSeconds;
}

void Rigidbody2D::ResetForce()
{
    m_LastFrameForce = Vec2::ZERO;
    m_LastFrameTorque = 0.f;
}
