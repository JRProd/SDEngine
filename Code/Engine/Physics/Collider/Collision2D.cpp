#include "Collision2D.hpp"

Collision2D Collision2D::GetInverted() const
{
    Collision2D inverted;
    inverted.self = this->other;
    inverted.other = this->self;

    inverted.manifold.contactEdge = this->manifold.contactEdge;
    inverted.manifold.penetration = -this->manifold.penetration;
    inverted.manifold.normal = -this->manifold.normal;
    return inverted;
}
