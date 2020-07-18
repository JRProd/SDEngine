#pragma once

#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Event/EventSystem.hpp"
#include "Engine/Event/Delegate.hpp"

#include "PhysicsMaterial.hpp"

#include <vector>
//-----------------------------------------------------------------------------
// Engine Predefines
struct Rgba8;
class Physics2D;
class Rigidbody2D;
class RenderContext;
struct Collision2D;

bool CommandDebugCollisionGJK( EventArgs* args );

enum Collider2DType
{
    COLLIDER_DISC,
    COLLIDER_POLYGON,

    NUM_COLLIDER_TYPES
};

class Collider2D
{
    friend class Physics2D;

public:
    Collider2DType m_Type;
    Rigidbody2D* m_Rigidbody = nullptr;

    Delegate<Collision2D> OnOverlapBegin;
    Delegate<Collision2D> OnOverlap;
    Delegate<Collision2D> OnOverlapEnd;

    Delegate<Collision2D> OnTriggerBegin;
    Delegate<Collision2D> OnTrigger;
    Delegate<Collision2D> OnTriggerEnd;

    virtual Vec2 GetClosestPoint( const Vec2& point ) const = 0;
    virtual Vec2 GetClosestPointOnHull( const Vec2& point ) const = 0;
    virtual bool Contains( const Vec2& point ) const = 0;
    virtual bool Intersects( Collider2D& other, OUT_PARAM Collision2D& collision);

    bool IsTrigger() const { return  m_IsTrigger; }
    void SetTrigger( bool newTriggerValue ) { m_IsTrigger = newTriggerValue; }

    virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) const = 0;

    virtual float CalculateMoment( float mass ) const = 0;
    virtual float GetRadius() const = 0;
    virtual Vec2 GetRightMostPoint() const = 0;
    virtual Vec2 GetTopMostPoint() const = 0;
    virtual Vec2 GetLeftMostPoint() const = 0;
    virtual Vec2 GetBottomMostPoint() const = 0;
    virtual std::vector<Vec2> GetPoints() const = 0;
    virtual Disc GetWorldBounds() const = 0;

    PhysicsMaterial GetPhysicsMaterial() const { return m_Material; }
    void SetPhysicsMaterial( PhysicsMaterial material ) { m_Material = material; };
    void SetCollisionLayer( unsigned int layer ) { m_CollisionLayer = layer; }
    float GetRestitutionAgainst( const Collider2D& other ) const;
    float GetFrictionAgainst( const Collider2D& other ) const;
    Vec2 GetWorldPosition() const { return m_WorldPosition; }

protected:
    Physics2D* m_PhysicsSystem = nullptr;
    PhysicsMaterial m_Material;
    Vec2 m_WorldPosition;

    unsigned int m_CollisionLayer = 0u;
    bool m_IsTrigger = false;

    Collider2D( Physics2D* physicsWorld );
    virtual ~Collider2D();

    void UpdateWorldShape();
private:
    bool m_DestroyRequested = false;

    bool FineRangeIntersection( Collider2D& other, OUT_PARAM Collision2D& collision );
    bool CollidesAgainst( unsigned int otherLayer ) const;
};

