#include "Engine.hpp"

#include "Engine/Core/Math/Primatives/Mat44.hpp"

// Defined Transform
static Mat44 g_CanonicalTransform;
static Mat44 g_CanonicalTransformInverse;
static Mat44 g_CanonicalTransformTranspose;

BitmapFont* Engine::g_Font = nullptr;

void Engine::SetCanonicalTransform( const Mat44& gameTransform )
{
    Mat44 transform = gameTransform;

    // Set the transform
    g_CanonicalTransform = transform;

    // Transpose and set the transpose, revert back after
    transform.Transpose();
    g_CanonicalTransformTranspose = transform;
    transform.Transpose();

    // Invert and set the inverse
    transform.Invert();
    g_CanonicalTransformInverse = transform;
}

const Mat44& Engine::GetCanonicalTransform()
{
    return g_CanonicalTransform;
}

const Mat44& Engine::GetCanonicalTransformInverse()
{
    return g_CanonicalTransformInverse;
}

const Mat44& Engine::GetCanonicalTransformTranspose()
{
    return g_CanonicalTransformTranspose;
}
