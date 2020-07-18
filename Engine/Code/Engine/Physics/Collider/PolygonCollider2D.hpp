#pragma once

#include <vector>
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Disc.hpp"
#include "Engine/Physics/Collider/Collider2D.hpp"

//-----------------------------------------------------------------------------
// Engine Predefines
struct LineSeg2D;

class PolygonCollider2D: public Collider2D
{
    friend class Physics2D;

public:
    Vec2 GetClosestPoint( const Vec2& point ) const override;
    Vec2 GetClosestPointOnHull(const Vec2& point) const override;
    bool Contains( const Vec2& point ) const override;
    void DebugRender( RenderContext* ctx, const Rgba8& borderColor,
                      const Rgba8& fillColor ) const override;

    float CalculateMoment(float mass) const override;
    float GetRadius() const override { return 0.f; }
    Vec2 GetSupportPoint( const Vec2& direction ) const;
    Vec2 GetRightMostPoint() const override;
    Vec2 GetTopMostPoint() const override;
    Vec2 GetLeftMostPoint() const override;
    Vec2 GetBottomMostPoint() const override;
    std::vector<Vec2> GetPoints() const override;
    Disc GetWorldBounds() const override;

    std::vector<Vec2> m_LocalTriangleCenters;
private:
    std::vector<float> m_TriangleMoments; 
    std::vector<Vec2> m_LocalPoints;
    Disc m_LocalBounds;
    float m_MasslessMoment = 0.f;

    PolygonCollider2D( Physics2D* physicsWorld, const std::vector<Vec2>& points, bool isCloud = true );

    // Clarification on `isPositiveProjectionNegative`
    //  Should points further from the start then the end of the LineSeg be classified as 
    //  on the negative side of the LineSeg.
    static bool PointOnNegativeSide( const LineSeg2D& lineSeg, const Vec2& testPoint,
                                     bool isPositiveProjectionNegative = false );

    void BuildFromInOrder( const std::vector<Vec2>& points );
    void BuildFromCloud( const std::vector<Vec2>& points );
    void FindCenterOfMassAndMoment();
    void LocalizePoints();
    void CalculateLocalBounds();

    Vec2 GetDirectionMostPoint( int direction ) const;
};
