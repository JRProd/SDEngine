#include "Mat44.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Utils/StringUtils.hpp"

Mat44::Mat44( float* sixteenFloatArray )
    : Ix( sixteenFloatArray[ 0 ] )
      , Iy( sixteenFloatArray[ 4 ] )
      , Iz( sixteenFloatArray[ 8 ] )
      , Iw( sixteenFloatArray[ 12 ] )

      , Jx( sixteenFloatArray[ 1 ] )
      , Jy( sixteenFloatArray[ 5 ] )
      , Jz( sixteenFloatArray[ 9 ] )
      , Jw( sixteenFloatArray[ 11 ] )

      , Kx( sixteenFloatArray[ 2 ] )
      , Ky( sixteenFloatArray[ 6 ] )
      , Kz( sixteenFloatArray[ 10 ] )
      , Kw( sixteenFloatArray[ 14 ] )

      , Tx( sixteenFloatArray[ 3 ] )
      , Ty( sixteenFloatArray[ 7 ] )
      , Tz( sixteenFloatArray[ 11 ] )
      , Tw( sixteenFloatArray[ 15 ] )
{
}

Mat44::Mat44( const Vec2& iBasis, const Vec2& jBasis, const Vec2& translation )
    : Ix( iBasis.x )
      , Iy( iBasis.y )

      , Jx( jBasis.x )
      , Jy( jBasis.y )

      , Tx( translation.x )
      , Ty( translation.y )
{
}

Mat44::Mat44( const Vec3& iBasis,
              const Vec3& jBasis,
              const Vec3& kBasis,
              const Vec3& translation )
    : Ix( iBasis.x )
      , Iy( iBasis.y )
      , Iz( iBasis.z )

      , Jx( jBasis.x )
      , Jy( jBasis.y )
      , Jz( jBasis.z )

      , Kx( kBasis.x )
      , Ky( kBasis.y )
      , Kz( kBasis.z )

      , Tx( translation.x )
      , Ty( translation.y )
      , Tz( translation.z )
{
}

Mat44::Mat44( const Vec4& iBasisHomogeneous,
              const Vec4& jBasisHomogeneous,
              const Vec4& kBasisHomogeneous,
              const Vec4& translationHomogeneous )
    : Ix( iBasisHomogeneous.x )
      , Iy( iBasisHomogeneous.y )
      , Iz( iBasisHomogeneous.z )
      , Iw( iBasisHomogeneous.w )

      , Jx( jBasisHomogeneous.x )
      , Jy( jBasisHomogeneous.y )
      , Jz( jBasisHomogeneous.z )
      , Jw( jBasisHomogeneous.w )

      , Kx( kBasisHomogeneous.x )
      , Ky( kBasisHomogeneous.y )
      , Kz( kBasisHomogeneous.z )
      , Kw( kBasisHomogeneous.w )

      , Tx( translationHomogeneous.x )
      , Ty( translationHomogeneous.y )
      , Tz( translationHomogeneous.z )
      , Tw( translationHomogeneous.w )
{
}

STATIC Mat44 Mat44::CreateFromXRotationDegrees( float xRotationDegrees )
{
    Mat44 xRotation = Mat44();
    xRotation.RotateXDegrees( xRotationDegrees );
    return xRotation;
}

STATIC Mat44 Mat44::CreateFromYRotationDegrees( float yRotationDegrees )
{
    Mat44 yRotation = Mat44();
    yRotation.RotateYDegrees( yRotationDegrees );
    return yRotation;
}

STATIC Mat44 Mat44::CreateFromZRotationDegrees( float zRotationDegrees )
{
    Mat44 zRotation = Mat44();
    zRotation.RotateZDegrees( zRotationDegrees );
    return zRotation;
}

// //-----------------------------------------------------------------------------
// // Creates a rotation based on euler angles representing pitch roll yaw
// // x-> pitch ( -90, 90 )
// // y-> yaw ( -180, 180 )
// // z-> roll ( -180, 180 )
// //
// STATIC Mat44 Mat44::CreateFromPitchRollYaw( const Vec3& euler )
// {
//     const Mat44 pitch = CreateFromXRotationDegrees( euler.pitch );
//     const Mat44 roll = CreateFromZRotationDegrees( euler.roll );
//     const Mat44 yaw = CreateFromYRotationDegrees( euler.yaw );
//     Mat44 identity;
//     identity.PushMatrix( yaw );
//     identity.PushMatrix( pitch );
//     identity.PushMatrix( roll );
//     return identity;
// }

STATIC Mat44 Mat44::CreateFromTranslation( const Vec2& transform )
{
    Mat44 transformMatrix = Mat44();
    transformMatrix.Translate( transform );
    return transformMatrix;
}

STATIC Mat44 Mat44::CreateFromTranslation( const Vec3& transform )
{
    Mat44 transformMatrix = Mat44();
    transformMatrix.Translate( transform );
    return transformMatrix;
}

STATIC Mat44 Mat44::CreateFromScaleUniform2D( float uniformScale )
{
    Mat44 scaleMatrix = Mat44();
    scaleMatrix.ScaleUniform2D( uniformScale );
    return scaleMatrix;
}

STATIC Mat44 Mat44::CreateFromScaleUniform3D( float uniformScale )
{
    Mat44 scaleMatrix = Mat44();
    scaleMatrix.ScaleUniform3D( uniformScale );
    return scaleMatrix;
}

STATIC Mat44 Mat44::CreateFromScale( const Vec2& scale )
{
    Mat44 scaleMatrix = Mat44();
    scaleMatrix.Scale( scale );
    return scaleMatrix;
}

STATIC Mat44 Mat44::CreateFromScale( const Vec3& scale )
{
    Mat44 scaleMatrix = Mat44();
    scaleMatrix.Scale( scale );
    return scaleMatrix;
}

STATIC Mat44 Mat44::CreateOrthographicProjection( const Vec3& min, const Vec3& max )
{
    Mat44 projection;
    const Vec3 difference = max - min;
    const Vec3 sum = max + min;

    projection.Ix = 2.f / difference.x;
    projection.Tx = -sum.x / difference.x;

    projection.Jy = 2.f / difference.y;
    projection.Ty = -sum.y / difference.y;

    projection.Kz = 1.f / difference.z;
    projection.Tz = -min.z / difference.z;

    return projection;
}

STATIC Mat44 Mat44::CreatePerspectiveProjection( float fieldOfView, float aspect, float farZ,
                                                 float nearZ )
{
    Mat44 projection;
    const float height = 1.f / tanfDegrees( fieldOfView * .5f );
    const float inverseRange = 1.f / (farZ - nearZ);

    // Screen is wider than it is tall
    if ( aspect >= 1 )
    {
        projection.Ix = height / aspect;
        projection.Jy = height;
    }
        // Screen is taller than it is wide
    else
    {
        projection.Ix = height;
        projection.Jy = aspect / height;
    }

    projection.Kz = -farZ * inverseRange;
    projection.Kw = -1.f;
    projection.Tz = (nearZ * farZ) * inverseRange;
    projection.Tw = 0.f;

    return projection;
}

Mat44 Mat44::LookAt( const Vec3& source, const Vec3& target, const Vec3& worldUp )
{
    const Vec3 forward = (target - source).GetNormalized();
    Vec3 right = forward.GetCross( worldUp );

    bool shouldNormalize = true;
    if ( IsMostlyEqual( right.GetLength(), 0.f ) )
    {
        right = forward.GetCross( Vec3::K );
        if( IsMostlyEqual( right.GetLength(), 0.f ) )
        {
            right = Vec3::I;
            shouldNormalize = false;
        }
    }

    if( shouldNormalize )
    {
        right.Normalize();
    }

    const Vec3& up = right.GetCross( forward );

    Mat44 lookAtMatrix;
    lookAtMatrix.SetBasis( right, up, -forward, source );
    return lookAtMatrix;
}

Mat44 Mat44::CreateFromTone( const Rgba8& tone )
{
    const Vec4 toneMapColumn = tone.GetAsVec4();
    Mat44 toneMap;
    toneMap.SetBasis( toneMapColumn, toneMapColumn, toneMapColumn, Vec4::T_BASIS );
    toneMap.Transpose();
    toneMap.SetTranslation( Vec4::T_BASIS );

    return toneMap;
}

Mat44 Mat44::CreateFromTint( const Rgba8& tint, float percent )
{
    Mat44 tintMap = Mat44::CreateFromScaleUniform3D( 1 - percent );
    const Vec3 tintMapColumn = tint.GetAsVec4().XYZ();
    tintMap.SetTranslation( percent * tintMapColumn );

    return tintMap;
}

Mat44 Mat44::CreateFromToneAndTint( const Rgba8& tone, const Rgba8& tint, const float percent )
{
    Mat44 toneMap = CreateFromTone( tone );
    const Mat44 tintMap = CreateFromTint( tint, percent );
    toneMap.PushMatrix( tintMap );

    return toneMap;
}

STATIC Mat44 Mat44::IDENTITY;

Vec2 Mat44::TransformVector( const Vec2& vectorQuantity ) const
{
    const Vec2& v = vectorQuantity;
    return Vec2( (Ix * v.x) + (Jx * v.y), (Iy * v.x) + (Jy * v.y) );
}

Vec3 Mat44::TransformVector( const Vec3& vectorQuantity ) const
{
    const Vec3& v = vectorQuantity;
    return Vec3( (Ix * v.x) + (Jx * v.y) + (Kx * v.z),
                 (Iy * v.x) + (Jy * v.y) + (Ky * v.z),
                 (Iz * v.x) + (Jz * v.y) + (Kz * v.z) );
}

Vec2 Mat44::TransformPosition( const Vec2& positionQuantity ) const
{
    const Vec2& v = positionQuantity;
    return Vec2( (v.x * Ix) + (v.y * Jx) + Tx,
                 (v.x * Iy) + (v.y * Jy) + Ty );
}

Vec3 Mat44::TransformPosition( const Vec3& positionQuantity ) const
{
    const Vec3& v = positionQuantity;
    return Vec3( (v.x * Ix) + (v.y * Jx) + (v.z * Kx) + Tx,
                 (v.x * Iy) + (v.y * Jy) + (v.z * Ky) + Ty,
                 (v.x * Iz) + (v.y * Jz) + (v.z * Kz) + Tz );
}

Vec4 Mat44::TransformHomogeneous( const Vec4& homogeneousPoint ) const
{
    const Vec4& v = homogeneousPoint;
    return Vec4( (v.x * Ix) + (v.y * Jx) + (v.z * Kx) + (v.w * Tx),
                 (v.x * Iy) + (v.y * Jy) + (v.z * Ky) + (v.w * Ty),
                 (v.x * Iz) + (v.y * Jz) + (v.z * Kz) + (v.w * Tz),
                 (v.x * Iw) + (v.y * Jw) + (v.z * Kw) + (v.w * Tw) );
}

Vec2 Mat44::GetIBasis2D() const
{
    return Vec2( Ix, Iy );
}

Vec3 Mat44::GetIBasis3D() const
{
    return Vec3( Ix, Iy, Iz );
}

Vec4 Mat44::GetIBasis4D() const
{
    return Vec4( Ix, Iy, Iz, Iw );
}

Vec2 Mat44::GetJBasis2D() const
{
    return Vec2( Jx, Jy );
}

Vec3 Mat44::GetJBasis3D() const
{
    return Vec3( Jx, Jy, Jz );
}

Vec4 Mat44::GetJBasis4D() const
{
    return Vec4( Jx, Jy, Jz, Jw );
}

Vec3 Mat44::GetKBasis3D() const
{
    return Vec3( Kx, Ky, Kz );
}

Vec4 Mat44::GetKBasis4D() const
{
    return Vec4( Kx, Ky, Kz, Kw );
}

Vec2 Mat44::GetTranslation2D() const
{
    return Vec2( Tx, Ty );
}

Vec3 Mat44::GetTranslation3D() const
{
    return Vec3( Tx, Ty, Tz );
}

Vec4 Mat44::GetTranslation4D() const
{
    return Vec4( Tx, Ty, Tz, Tw );
}

bool Mat44::IsOrthonormal() const
{
    const Vec3 iBasis = GetIBasis3D();
    const Vec3 jBasis = GetJBasis3D();
    const Vec3 kBasis = GetKBasis3D();

    const float ijDot = Vec3::Dot( iBasis, jBasis );
    const float ikDot = Vec3::Dot( iBasis, kBasis );
    const float jkDot = Vec3::Dot( jBasis, kBasis );

    if ( !IsMostlyEqual( ijDot, 0.f, 1e-5f ) ||
        !IsMostlyEqual( ikDot, 0.f, 1e-5f ) ||
        !IsMostlyEqual( jkDot, 0.f, 1e-5f ) )
    {
        return false;
    }

    const float iLength = iBasis.GetLengthSquared();
    const float jLength = jBasis.GetLengthSquared();
    const float kLength = kBasis.GetLengthSquared();

    if ( !IsMostlyEqual( iLength, 1.f, 1e-5f ) ||
        !IsMostlyEqual( jLength, 1.f, 1e-5f ) ||
        !IsMostlyEqual( kLength, 1.f, 1e-5f ) )
    {
        return false;
    }

    return true;
}

void Mat44::SetTranslation( const Vec2& newTranslation )
{
    Tx = newTranslation.x;
    Ty = newTranslation.y;
}

void Mat44::SetTranslation( const Vec3& newTranslation )
{
    Tx = newTranslation.x;
    Ty = newTranslation.y;
    Tz = newTranslation.z;
}

void Mat44::SetTranslation( const Vec4& newTranslation )
{
    Tx = newTranslation.x;
    Ty = newTranslation.y;
    Tz = newTranslation.z;
    Tw = newTranslation.w;
}

void Mat44::SetBasis( const Vec2& iBasis, const Vec2& jBasis )
{
    Ix = iBasis.x;
    Iy = iBasis.y;

    Jx = jBasis.x;
    Jy = jBasis.y;
}

void Mat44::SetBasis( const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform )
{
    SetBasis( iBasis, jBasis );
    SetTranslation( transform );
}

void Mat44::SetBasis( const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis )
{
    Ix = iBasis.x;
    Iy = iBasis.y;
    Iz = iBasis.z;

    Jx = jBasis.x;
    Jy = jBasis.y;
    Jz = jBasis.z;

    Kx = kBasis.x;
    Ky = kBasis.y;
    Kz = kBasis.z;
}

void Mat44::SetBasis( const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis,
                      const Vec3& transform )
{
    SetBasis( iBasis, jBasis, kBasis );
    SetTranslation( transform );
}

void Mat44::SetBasis( const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis,
                      const Vec4& transform )
{
    Ix = iBasis.x;
    Iy = iBasis.y;
    Iz = iBasis.z;
    Iw = iBasis.w;

    Jx = jBasis.x;
    Jy = jBasis.y;
    Jz = jBasis.z;
    Jw = jBasis.w;

    Kx = kBasis.x;
    Ky = kBasis.y;
    Kz = kBasis.z;
    Kw = kBasis.w;

    SetTranslation( transform );
}

void Mat44::SetIBasis( const Vec4& iBasis )
{
    Ix = iBasis.x;
    Iy = iBasis.y;
    Iz = iBasis.z;
    Iw = iBasis.w;
}

void Mat44::SetJBasis( const Vec4& jBasis )
{
    Jx = jBasis.x;
    Jy = jBasis.y;
    Jz = jBasis.z;
    Jw = jBasis.w;
}

void Mat44::SetKBasis( const Vec4& kBasis )
{
    Kx = kBasis.x;
    Ky = kBasis.y;
    Kz = kBasis.z;
    Kw = kBasis.w;
}

void Mat44::SetTBasis( const Vec4& tBasis )
{
    Tx = tBasis.x;
    Ty = tBasis.y;
    Tz = tBasis.z;
    Tw = tBasis.w;
}

void Mat44::RotateXDegrees( float degreesAroundX )
{
    const float JxCopy = Jx;
    const float JyCopy = Jy;
    const float JzCopy = Jz;
    const float JwCopy = Jw;

    const float c = cosfDegrees( degreesAroundX );
    const float s = sinfDegrees( degreesAroundX );

    Jx = (JxCopy * c) + (Kx * s);
    Jy = (JyCopy * c) + (Ky * s);
    Jz = (JzCopy * c) + (Kz * s);
    Jw = (JwCopy * c) + (Kw * s);

    Kx = (JxCopy * -s) + (Kx * c);
    Ky = (JyCopy * -s) + (Ky * c);
    Kz = (JzCopy * -s) + (Kz * c);
    Kw = (JwCopy * -s) + (Kw * c);
}

void Mat44::RotateYDegrees( float degreesAroundY )
{
    const float IxCopy = Ix;
    const float IyCopy = Iy;
    const float IzCopy = Iz;
    const float IwCopy = Iw;

    const float c = cosfDegrees( degreesAroundY );
    const float s = sinfDegrees( degreesAroundY );

    Ix = (IxCopy * c) + (Kx * -s);
    Iy = (IyCopy * c) + (Ky * -s);
    Iz = (IzCopy * c) + (Kz * -s);
    Iw = (IwCopy * c) + (Kw * -s);

    Kx = (IxCopy * s) + (Kx * c);
    Ky = (IyCopy * s) + (Ky * c);
    Kz = (IzCopy * s) + (Kz * c);
    Kw = (IwCopy * s) + (Kw * c);
}

void Mat44::RotateZDegrees( float degreesAroundZ )
{
    const float IxCopy = Ix;
    const float IyCopy = Iy;
    const float IzCopy = Iz;
    const float IwCopy = Iw;

    const float c = cosfDegrees( degreesAroundZ );
    const float s = sinfDegrees( degreesAroundZ );

    Ix = (IxCopy * c) + (Jx * s);
    Iy = (IyCopy * c) + (Jy * s);
    Iz = (IzCopy * c) + (Jz * s);
    Iw = (IwCopy * c) + (Jw * s);

    Jx = (IxCopy * -s) + (Jx * c);
    Jy = (IyCopy * -s) + (Jy * c);
    Jz = (IzCopy * -s) + (Jz * c);
    Jw = (IwCopy * -s) + (Jw * c);
}

void Mat44::Translate( const Vec2& translation )
{
    Tx = (Ix * translation.x) + (Jx * translation.y) + Tx;
    Ty = (Iy * translation.x) + (Jy * translation.y) + Ty;
    Tz = (Iz * translation.x) + (Jz * translation.y) + Tz;
    Tw = (Iw * translation.x) + (Jw * translation.y) + Tw;
}

void Mat44::Translate( const Vec3& translation )
{
    Tx = (Ix * translation.x) + (Jx * translation.y) + (Kx * translation.z) + Tx;
    Ty = (Iy * translation.x) + (Jy * translation.y) + (Ky * translation.z) + Ty;
    Tz = (Iz * translation.x) + (Jz * translation.y) + (Kz * translation.z) + Tz;
    Tw = (Iw * translation.x) + (Jw * translation.y) + (Kw * translation.z) + Tw;
}

void Mat44::ScaleUniform2D( float uniformScale )
{
    Ix *= uniformScale;
    Iy *= uniformScale;
    Iz *= uniformScale;
    Iw *= uniformScale;

    Jx *= uniformScale;
    Jy *= uniformScale;
    Jz *= uniformScale;
    Jw *= uniformScale;
}

void Mat44::ScaleUniform3D( float uniformScale )
{
    Ix *= uniformScale;
    Iy *= uniformScale;
    Iz *= uniformScale;
    Iw *= uniformScale;

    Jx *= uniformScale;
    Jy *= uniformScale;
    Jz *= uniformScale;
    Jw *= uniformScale;

    Kx *= uniformScale;
    Ky *= uniformScale;
    Kz *= uniformScale;
    Kw *= uniformScale;
}

void Mat44::Scale( const Vec2& scale )
{
    Ix *= scale.x;
    Iy *= scale.x;
    Iz *= scale.x;
    Iw *= scale.x;

    Jx *= scale.y;
    Jy *= scale.y;
    Jz *= scale.y;
    Jw *= scale.y;
}

void Mat44::Scale( const Vec3& scale )
{
    Ix *= scale.x;
    Iy *= scale.x;
    Iz *= scale.x;
    Iw *= scale.x;

    Jx *= scale.y;
    Jy *= scale.y;
    Jz *= scale.y;
    Jw *= scale.y;

    Kx *= scale.z;
    Ky *= scale.z;
    Kz *= scale.z;
    Kw *= scale.z;
}

void Mat44::Transpose()
{
    float temp = Iy;
    Iy = Jx;
    Jx = temp;

    temp = Iz;
    Iz = Kx;
    Kx = temp;

    temp = Iw;
    Iw = Tx;
    Tx = temp;

    temp = Jz;
    Jz = Ky;
    Ky = temp;

    temp = Jw;
    Jw = Ty;
    Ty = temp;

    temp = Kw;
    Kw = Tz;
    Tz = temp;
}

void Mat44::PushMatrix( const Mat44& arbitraryTransform )
{
    const Mat44 a = *this;
    const Mat44& t = arbitraryTransform;

    Ix = (a.Ix * t.Ix) + (a.Jx * t.Iy) + (a.Kx * t.Iz) + (a.Tx * t.Iw);
    Iy = (a.Iy * t.Ix) + (a.Jy * t.Iy) + (a.Ky * t.Iz) + (a.Ty * t.Iw);
    Iz = (a.Iz * t.Ix) + (a.Jz * t.Iy) + (a.Kz * t.Iz) + (a.Tz * t.Iw);
    Iw = (a.Iw * t.Ix) + (a.Jw * t.Iy) + (a.Kw * t.Iz) + (a.Tw * t.Iw);

    Jx = (a.Ix * t.Jx) + (a.Jx * t.Jy) + (a.Kx * t.Jz) + (a.Tx * t.Jw);
    Jy = (a.Iy * t.Jx) + (a.Jy * t.Jy) + (a.Ky * t.Jz) + (a.Ty * t.Jw);
    Jz = (a.Iz * t.Jx) + (a.Jz * t.Jy) + (a.Kz * t.Jz) + (a.Tz * t.Jw);
    Jw = (a.Iw * t.Jx) + (a.Jw * t.Jy) + (a.Kw * t.Jz) + (a.Tw * t.Jw);

    Kx = (a.Ix * t.Kx) + (a.Jx * t.Ky) + (a.Kx * t.Kz) + (a.Tx * t.Kw);
    Ky = (a.Iy * t.Kx) + (a.Jy * t.Ky) + (a.Ky * t.Kz) + (a.Ty * t.Kw);
    Kz = (a.Iz * t.Kx) + (a.Jz * t.Ky) + (a.Kz * t.Kz) + (a.Tz * t.Kw);
    Kw = (a.Iw * t.Kx) + (a.Jw * t.Ky) + (a.Kw * t.Kz) + (a.Tw * t.Kw);

    Tx = (a.Ix * t.Tx) + (a.Jx * t.Ty) + (a.Kx * t.Tz) + (a.Tx * t.Tw);
    Ty = (a.Iy * t.Tx) + (a.Jy * t.Ty) + (a.Ky * t.Tz) + (a.Ty * t.Tw);
    Tz = (a.Iz * t.Tx) + (a.Jz * t.Ty) + (a.Kz * t.Tz) + (a.Tz * t.Tw);
    Tw = (a.Iw * t.Tx) + (a.Jw * t.Ty) + (a.Kw * t.Tz) + (a.Tw * t.Tw);
}

void Mat44::InvertOrthonormal()
{
    GUARANTEE_OR_DIE( IsOrthonormal(),
                      "InvertOrthonormal: Matrix is not orthonormal. Cannot invert with this method" );

    Vec3 translation = GetTranslation3D();
    SetTranslation( Vec3::ZERO );
    Transpose();
    translation = TransformVector( -translation );

    SetTranslation( translation );
}

float& Mat44::operator[]( size_t index )
{
    switch ( index )
    {
    case 0: return Ix;
    case 1: return Iy;
    case 2: return Iz;
    case 3: return Iw;
    case 4: return Jx;
    case 5: return Jy;
    case 6: return Jz;
    case 7: return Jw;
    case 8: return Kx;
    case 9: return Ky;
    case 10: return Kz;
    case 11: return Kw;
    case 12: return Tx;
    case 13: return Ty;
    case 14: return Tz;
    case 15: return Tw;
    default: ERROR_AND_DIE( Stringf("Mat44[]: Index (%d) out of bounds", index) );
    }
}

bool Mat44::Invert()
{
    const Mat44 copy = *this;

    this->Ix = copy.Jy * copy.Kz * copy.Tw -
        copy.Jy * copy.Kw * copy.Tz -
        copy.Ky * copy.Jz * copy.Tw +
        copy.Ky * copy.Jw * copy.Tz +
        copy.Ty * copy.Jz * copy.Kw -
        copy.Ty * copy.Jw * copy.Kz;

    this->Jx = -copy.Jx * copy.Kz * copy.Tw +
        copy.Jx * copy.Kw * copy.Tz +
        copy.Kx * copy.Jz * copy.Tw -
        copy.Kx * copy.Jw * copy.Tz -
        copy.Tx * copy.Jz * copy.Kw +
        copy.Tx * copy.Jw * copy.Kz;

    this->Kx = copy.Jx * copy.Ky * copy.Tw -
        copy.Jx * copy.Kw * copy.Ty -
        copy.Kx * copy.Jy * copy.Tw +
        copy.Kx * copy.Jw * copy.Ty +
        copy.Tx * copy.Jy * copy.Kw -
        copy.Tx * copy.Jw * copy.Ky;

    this->Tx = -copy.Jx * copy.Ky * copy.Tz +
        copy.Jx * copy.Kz * copy.Ty +
        copy.Kx * copy.Jy * copy.Tz -
        copy.Kx * copy.Jz * copy.Ty -
        copy.Tx * copy.Jy * copy.Kz +
        copy.Tx * copy.Jz * copy.Ky;

    this->Iy = -copy.Iy * copy.Kz * copy.Tw +
        copy.Iy * copy.Kw * copy.Tz +
        copy.Ky * copy.Iz * copy.Tw -
        copy.Ky * copy.Iw * copy.Tz -
        copy.Ty * copy.Iz * copy.Kw +
        copy.Ty * copy.Iw * copy.Kz;

    this->Jy = copy.Ix * copy.Kz * copy.Tw -
        copy.Ix * copy.Kw * copy.Tz -
        copy.Kx * copy.Iz * copy.Tw +
        copy.Kx * copy.Iw * copy.Tz +
        copy.Tx * copy.Iz * copy.Kw -
        copy.Tx * copy.Iw * copy.Kz;

    this->Ky = -copy.Ix * copy.Ky * copy.Tw +
        copy.Ix * copy.Kw * copy.Ty +
        copy.Kx * copy.Iy * copy.Tw -
        copy.Kx * copy.Iw * copy.Ty -
        copy.Tx * copy.Iy * copy.Kw +
        copy.Tx * copy.Iw * copy.Ky;

    this->Ty = copy.Ix * copy.Ky * copy.Tz -
        copy.Ix * copy.Kz * copy.Ty -
        copy.Kx * copy.Iy * copy.Tz +
        copy.Kx * copy.Iz * copy.Ty +
        copy.Tx * copy.Iy * copy.Kz -
        copy.Tx * copy.Iz * copy.Ky;

    this->Iz = copy.Iy * copy.Jz * copy.Tw -
        copy.Iy * copy.Jw * copy.Tz -
        copy.Jy * copy.Iz * copy.Tw +
        copy.Jy * copy.Iw * copy.Tz +
        copy.Ty * copy.Iz * copy.Jw -
        copy.Ty * copy.Iw * copy.Jz;

    this->Jz = -copy.Ix * copy.Jz * copy.Tw +
        copy.Ix * copy.Jw * copy.Tz +
        copy.Jx * copy.Iz * copy.Tw -
        copy.Jx * copy.Iw * copy.Tz -
        copy.Tx * copy.Iz * copy.Jw +
        copy.Tx * copy.Iw * copy.Jz;

    this->Kz = copy.Ix * copy.Jy * copy.Tw -
        copy.Ix * copy.Jw * copy.Ty -
        copy.Jx * copy.Iy * copy.Tw +
        copy.Jx * copy.Iw * copy.Ty +
        copy.Tx * copy.Iy * copy.Jw -
        copy.Tx * copy.Iw * copy.Jy;

    this->Tz = -copy.Ix * copy.Jy * copy.Tz +
        copy.Ix * copy.Jz * copy.Ty +
        copy.Jx * copy.Iy * copy.Tz -
        copy.Jx * copy.Iz * copy.Ty -
        copy.Tx * copy.Iy * copy.Jz +
        copy.Tx * copy.Iz * copy.Jy;

    this->Iw = -copy.Iy * copy.Jz * copy.Kw +
        copy.Iy * copy.Jw * copy.Kz +
        copy.Jy * copy.Iz * copy.Kw -
        copy.Jy * copy.Iw * copy.Kz -
        copy.Ky * copy.Iz * copy.Jw +
        copy.Ky * copy.Iw * copy.Jz;

    this->Jw = copy.Ix * copy.Jz * copy.Kw -
        copy.Ix * copy.Jw * copy.Kz -
        copy.Jx * copy.Iz * copy.Kw +
        copy.Jx * copy.Iw * copy.Kz +
        copy.Kx * copy.Iz * copy.Jw -
        copy.Kx * copy.Iw * copy.Jz;

    this->Kw = -copy.Ix * copy.Jy * copy.Kw +
        copy.Ix * copy.Jw * copy.Ky +
        copy.Jx * copy.Iy * copy.Kw -
        copy.Jx * copy.Iw * copy.Ky -
        copy.Kx * copy.Iy * copy.Jw +
        copy.Kx * copy.Iw * copy.Jy;

    this->Tw = copy.Ix * copy.Jy * copy.Kz -
        copy.Ix * copy.Jz * copy.Ky -
        copy.Jx * copy.Iy * copy.Kz +
        copy.Jx * copy.Iz * copy.Ky +
        copy.Kx * copy.Iy * copy.Jz -
        copy.Kx * copy.Iz * copy.Jy;

    float det = copy.Ix * this->Ix + copy.Iy * this->Jx + copy.Iz * this->Kx + copy.Iw * this->Tx;

    if ( det == 0 )
        return false;

    det = 1.f / det;

    for ( size_t i = 0; i < 16; i++ )
    {
        operator[]( i ) = operator[]( i ) * det;
    }

    return true;
}

bool Mat44::operator==( const Mat44& rhs ) const
{
    return !operator!=( rhs );
}

bool Mat44::operator!=( const Mat44& rhs ) const
{
    return Ix != rhs.Ix || Iy != rhs.Iy || Iz != rhs.Iz || Iw != rhs.Iw || 
        Jx != rhs.Jx || Jy != rhs.Jy || Jz != rhs.Jz || Iw != rhs.Jw ||
        Kx != rhs.Kx || Ky != rhs.Ky || Kz != rhs.Kz || Iw != rhs.Kw ||
        Tx != rhs.Tx || Ty != rhs.Ty || Tz != rhs.Tz || Iw != rhs.Tw;
}
