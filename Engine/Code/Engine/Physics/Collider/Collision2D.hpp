#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"

class Collider2D;

struct Manifold2D
{
    Vec2 normal = Vec2::UNIT_EAST;
    LineSeg2D contactEdge;
    float penetration = 0.f;
};

struct Collision2D
{
    Collider2D* self = nullptr;
    Collider2D* other = nullptr;

    Manifold2D manifold;

public:
    Collision2D() = default;

    Collision2D GetInverted() const;
};