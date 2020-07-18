#include "Engine/Physics/Collider/DiscCollider2D.hpp"

#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"

DiscCollider2D::DiscCollider2D( Physics2D* physicsWorld, const Disc& disc )
    :   Collider2D( physicsWorld ) , m_LocalDisc( disc )
{
}

Vec2 DiscCollider2D::GetClosestPoint( const Vec2& point ) const
{
    Disc inWorldDisc = m_LocalDisc;
    inWorldDisc.Translate( m_WorldPosition );

    return inWorldDisc.GetNearestPointInDisc( point );
}

Vec2 DiscCollider2D::GetClosestPointOnHull( const Vec2& point ) const
{
    Disc inWorldDisc = m_LocalDisc;
    inWorldDisc.center += m_WorldPosition;

    Vec2 centerToPoint = inWorldDisc.center - point;
    centerToPoint.SetLength( inWorldDisc.radius );
    return centerToPoint;
}

bool DiscCollider2D::Contains( const Vec2& point ) const
{
    Disc inWorldDisc = m_LocalDisc;
    inWorldDisc.Translate( m_WorldPosition );

    return inWorldDisc.IsPointInside( point );
}

void DiscCollider2D::DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) const
{
    std::vector<VertexMaster> discVisual;
    Disc worldDisc = m_LocalDisc;
    worldDisc.center += m_WorldPosition;
    const Vec2 radiusLine = Vec2( m_LocalDisc.radius * cosf( m_Rigidbody->GetAngleRadians() ),
                            m_LocalDisc.radius * sinf( m_Rigidbody->GetAngleRadians() ) );

    AppendDisc( discVisual, worldDisc, fillColor );
    AppendDiscPerimeter( discVisual, worldDisc, borderColor, .25f );
    AppendLine( discVisual, LineSeg2D(worldDisc.center, worldDisc.center + radiusLine), borderColor, .15f );

    ctx->BindTexture( nullptr );
    ctx->DrawVertexArray( discVisual );
}

float DiscCollider2D::CalculateMoment( float mass ) const
{
    return mass * GetRadius() * GetRadius() * .5f;
}

//-----------------------------------------------------------------------------
Vec2 DiscCollider2D::GetRightMostPoint() const
{
    const Vec2 rightMost = m_LocalDisc.center;
    return rightMost + Vec2( m_LocalDisc.radius, 0.f );
}

//-----------------------------------------------------------------------------
Vec2 DiscCollider2D::GetTopMostPoint() const
{
    const Vec2 topMost = m_LocalDisc.center;
    return topMost + Vec2( 0.f, m_LocalDisc.radius );
}

//-----------------------------------------------------------------------------
Vec2 DiscCollider2D::GetLeftMostPoint() const
{
    const Vec2 leftMost = m_LocalDisc.center;
    return leftMost + Vec2( -m_LocalDisc.radius, 0.f );
}

//-----------------------------------------------------------------------------
Vec2 DiscCollider2D::GetBottomMostPoint() const
{
    const Vec2 bottomMost = m_LocalDisc.center;
    return bottomMost + Vec2( 0.f, -m_LocalDisc.radius );
}

std::vector<Vec2> DiscCollider2D::GetPoints() const
{
    std::vector<Vec2> points;
    points.push_back( m_LocalDisc.center + m_WorldPosition );
    return points;
}

Disc DiscCollider2D::GetWorldBounds() const
{
    Disc worldDisc = m_LocalDisc;
    worldDisc.center += m_WorldPosition;
    return worldDisc;
}


