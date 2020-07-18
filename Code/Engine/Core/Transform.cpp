#include "Transform.hpp"


#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"

#include <ThirdParty/stb/stb_image.h>

Transform::Transform( const Vec3& pos, const Vec3& scl, const Vec3& rotationOnAxis )
    : position( pos )
      , rotationAroundAxis( rotationOnAxis )
      , scale( scl )
{
}

Mat44 Transform::GetAsMatrix() const
{
    GUARANTEE_RECOVERABLE( scale.x > 0 && scale.y > 0 && scale.z > 0, "Transform: Scale is zero" );

    Mat44 transform = Mat44::CreateFromTranslation( position );
    transform.PushMatrix( GetRotationMatrix( ) );
    transform.PushMatrix( Mat44::CreateFromScale( scale ) );

    return transform;
}

Mat44 Transform::GetAsMatrixWithCanonicalTransform() const
{
    GUARANTEE_RECOVERABLE( scale.x > 0 && scale.y > 0 && scale.z > 0, "Transform: Scale is zero" );

    Mat44 transform = Mat44::CreateFromTranslation( position );
    transform.PushMatrix( GetRotationMatrix( true ) );
    transform.PushMatrix( Mat44::CreateFromScale( scale ) );

    return transform;
}

void Transform::SetRotationFromAxis( const Vec3& rotationOnAxis )
{
    rotationAroundAxis.x = rotationOnAxis.x;
    rotationAroundAxis.y = rotationOnAxis.y;
    rotationAroundAxis.z = rotationOnAxis.z;
}

void Transform::AppendRotationFromAxis( const float xAxis, const float yAxis, const float zAxis )
{
    rotationAroundAxis.x += xAxis;
    rotationAroundAxis.y += yAxis;
    rotationAroundAxis.z += zAxis;
}

void Transform::AppendRotationFromAxis( const Vec3& deltaAxisRotation )
{
    rotationAroundAxis.x += deltaAxisRotation.x;
    rotationAroundAxis.y += deltaAxisRotation.y;
    rotationAroundAxis.z += deltaAxisRotation.z;
}

Mat44 Transform::GetRotationMatrix( bool pushCanonical ) const
{
    Mat44 rotationFinal = Mat44::CreateFromZRotationDegrees( rotationAroundAxis.z );
    rotationFinal.PushMatrix( Mat44::CreateFromYRotationDegrees( rotationAroundAxis.y ) );
    rotationFinal.PushMatrix( Mat44::CreateFromXRotationDegrees( rotationAroundAxis.x ) );

    if( pushCanonical )
    {
        rotationFinal.PushMatrix( Engine::GetCanonicalTransform() );
    }

    return rotationFinal;
}
