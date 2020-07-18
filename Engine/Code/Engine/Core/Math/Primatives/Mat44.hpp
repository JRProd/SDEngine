#pragma once

#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Math/Primatives/Vec4.hpp"

struct Rgba8;

struct Mat44
{
    float Ix = 1.f;
    float Iy = 0.f;
    float Iz = 0.f;
    float Iw = 0.f;

    float Jx = 0.f;
    float Jy = 1.f;
    float Jz = 0.f;
    float Jw = 0.f;

    float Kx = 0.f;
    float Ky = 0.f;
    float Kz = 1.f;
    float Kw = 0.f;

    float Tx = 0.f;
    float Ty = 0.f;
    float Tz = 0.f;
    float Tw = 1.f;

public:
    Mat44() = default;
    explicit Mat44( float* sixteenFloatArray );
    explicit Mat44( const Vec2& iBasis,
                    const Vec2& jBasis,
                    const Vec2& translation );
    explicit Mat44( const Vec3& iBasis,
                    const Vec3& jBasis,
                    const Vec3& kBasis,
                    const Vec3& translation );
    explicit Mat44( const Vec4& iBasisHomogeneous,
                    const Vec4& jBasisHomogeneous,
                    const Vec4& kBasisHomogeneous,
                    const Vec4& translationHomogeneous );

    //-------------------------------------------------------------------------
    // Static Methods
    static Mat44 CreateFromXRotationDegrees( float xRotationDegrees );
    static Mat44 CreateFromYRotationDegrees( float yRotationDegrees );
    static Mat44 CreateFromZRotationDegrees( float zRotationDegrees );

    static Mat44 CreateFromTranslation( const Vec2& transform );
    static Mat44 CreateFromTranslation( const Vec3& transform );

    static Mat44 CreateFromScaleUniform2D( float uniformScale );
    static Mat44 CreateFromScaleUniform3D( float uniformScale );
    static Mat44 CreateFromScale( const Vec2& scale );
    static Mat44 CreateFromScale( const Vec3& scale );

    static Mat44 CreateOrthographicProjection( const Vec3& min, const Vec3& max );
    static Mat44 CreatePerspectiveProjection( float fieldOfView, float aspect, float farZ, float nearZ = -1.f );

    static Mat44 LookAt( const Vec3& source, const Vec3& target, const Vec3& worldUp = Vec3::J );

    static Mat44 CreateFromTone( const Rgba8& tone );
    static Mat44 CreateFromTint( const Rgba8& tint, float percent );
    static Mat44 CreateFromToneAndTint( const Rgba8& tone, const Rgba8& tint, float percent );

    // Static Mat44s
    static Mat44 IDENTITY;

    //-------------------------------------------------------------------------
    // Vector Transformations
    Vec2 TransformVector( const Vec2& vectorQuantity ) const;
    Vec3 TransformVector( const Vec3& vectorQuantity ) const;
    Vec2 TransformPosition( const Vec2& positionQuantity ) const;
    Vec3 TransformPosition( const Vec3& positionQuantity ) const;
    Vec4 TransformHomogeneous( const Vec4& homogeneousPoint ) const;

    //-------------------------------------------------------------------------
    // Basic accessors
    const float* GetReadonlyFloatArray() const { return &Ix; }
    float* GetFloatArray() { return &Ix; }

    Vec2 GetIBasis2D() const;
    Vec3 GetIBasis3D() const;
    Vec4 GetIBasis4D() const;

    Vec2 GetJBasis2D() const;
    Vec3 GetJBasis3D() const;
    Vec4 GetJBasis4D() const;

    Vec3 GetKBasis3D() const;
    Vec4 GetKBasis4D() const;

    Vec2 GetTranslation2D() const;
    Vec3 GetTranslation3D() const;
    Vec4 GetTranslation4D() const;

    bool IsOrthonormal() const;

    //-------------------------------------------------------------------------
    // Basic mutators
    void SetTranslation( const Vec2& newTranslation );
    void SetTranslation( const Vec3& newTranslation );
    void SetTranslation( const Vec4& newTranslation );

    void SetBasis( const Vec2& iBasis, const Vec2& jBasis );
    void SetBasis( const Vec2& iBasis, const Vec2& jBasis, const Vec2& transform );
    void SetBasis( const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis );
    void SetBasis( const Vec3& iBasis, const Vec3& jBasis, const Vec3& kBasis, const Vec3& transform );
    void SetBasis( const Vec4& iBasis, const Vec4& jBasis, const Vec4& kBasis, const Vec4& transform );

    void SetIBasis( const Vec4& iBasis );
    void SetJBasis( const Vec4& jBasis );
    void SetKBasis( const Vec4& kBasis );
    void SetTBasis( const Vec4& tBasis );

    //-------------------------------------------------------------------------
    // Transform mutators
    void RotateXDegrees( float degreesAroundX );
    void RotateYDegrees( float degreesAroundY );
    void RotateZDegrees( float degreesAroundZ );

    void Translate( const Vec2& translation );
    void Translate( const Vec3& translation );

    void ScaleUniform2D( float uniformScale );
    void ScaleUniform3D( float uniformScale );
    void Scale( const Vec2& scale );
    void Scale( const Vec3& scale );

    void Transpose();
    void PushMatrix( const Mat44& arbitraryTransform );

    void InvertOrthonormal();
    bool Invert();

    bool operator==( const Mat44& rhs ) const;
    bool operator!=( const Mat44& rhs ) const;

private:
    float& operator[](size_t index);
    const Mat44 operator*( const Mat44& rhs ) const = delete;
};
