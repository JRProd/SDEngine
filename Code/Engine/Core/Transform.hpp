#pragma once

#include "Math/Primatives/Mat44.hpp"
#include "Math/Primatives/Vec3.hpp"

class InputSystem;

struct Transform
{
    Vec3 position = Vec3::ZERO;
    Vec3 rotationAroundAxis = Vec3::ZERO;
    Vec3 scale = Vec3::ONE;

public:
    Transform() = default;
    explicit Transform( const Vec3& pos, const Vec3& scl, const Vec3& rotationOnAxis );

    Mat44 GetAsMatrix() const;
    Mat44 GetAsMatrixWithCanonicalTransform() const;

    //-----------------------------------------------------------------------------
    // Modifiers
    void Translate( const Vec3& translation ) { position += translation; }
    void SetRotationFromAxis( const Vec3& rotationOnAxis );
    void AppendRotationFromAxis( float xAxis, float yAxis, float zAxis );
    void AppendRotationFromAxis( const Vec3& deltaAxisRotation );

private:
    Mat44 GetRotationMatrix( bool pushCanonical = false ) const;
};
