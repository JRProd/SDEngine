#pragma once

#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Physics/Collider/Collider2D.hpp"

class DiscCollider2D: public Collider2D
{
    friend class Physics2D;

public:
    Disc m_LocalDisc;

    Vec2 GetClosestPoint( const Vec2& point ) const override;
    Vec2 GetClosestPointOnHull(const Vec2& point) const override;
    bool Contains( const Vec2& point ) const override;

    void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) const override;

    float CalculateMoment(float mass) const override;
    float GetRadius() const override { return m_LocalDisc.radius; }
    Vec2 GetRightMostPoint() const override;
    Vec2 GetTopMostPoint() const override;
    Vec2 GetLeftMostPoint() const override;
    Vec2 GetBottomMostPoint() const override;
    std::vector<Vec2> GetPoints() const override;
    Disc GetWorldBounds() const override;

private:
    DiscCollider2D( Physics2D* physicsWorld, const Disc& disc );

};