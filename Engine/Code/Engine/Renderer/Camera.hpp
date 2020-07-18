#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/Mat44.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Transform.hpp"

#include <vector>
#include "Engine/Core/Math/Primatives/AABB3.hpp"


#include "Buffers/EngineBufferData.hpp"

enum OutsideCameraViewBit: unsigned char
{
    OUTSIDE_VIEW_RIGHT = 1 << 0,
    OUTSIDE_VIEW_TOP = 1 << 1,
    OUTSIDE_VIEW_LEFT = 1 << 2,
    OUTSIDE_VIEW_BOTTOM = 1 << 3,
    OUTSIDE_VIEW_RIGHT_COMPLETELY = 1 << 4,
    OUTSIDE_VIEW_TOP_COMPLETELY = 1 << 5,
    OUTSIDE_VIEW_LEFT_COMPLETELY = 1 << 6,
    OUTISDE_VIEW_BOTTOM_COMPLETELY = 1 << 7,
};

typedef unsigned char OutsideCameraView;

//-----------------------------------------------------------------------------
// Engine Predefines
class Texture;
class RenderBuffer;
class RenderContext;

enum CameraClearBitFlag : unsigned int
{
    CLEAR_NONE_BIT = 0u,

    CLEAR_COLOR_BIT = BIT_FLAG<0>,
    CLEAR_DEPTH_BIT = BIT_FLAG<1>,
    CLEAR_STENCIL_BIT = BIT_FLAG<2>,
};

class Camera
{
public:
    Camera( RenderContext* owner );
    ~Camera();

    void SetOutputSize( const Vec2& outputSize );
    void SetCameraPosition( const Vec3& cameraPosition );
    void SetDepthStencilBuffer( Texture* depthStencilTexture ) { m_DepthStencilBuffer = depthStencilTexture; }

    void SetProjectionMatrix( const Mat44& matrix );
    void SetProjectionOrthographic( const AABB2& bounds, float nearZ = 1.f, float farZ = -1.f );
    void SetProjectionOrthographic( float height, float nearZ = 1.f, float farZ = -1.f );
    void SetProjectionOrthographicXYPlane( const AABB2& bounds, float nearZ = 1.f, float farZ = -1.f );
    void SetProjectionOrthographicXYPlane( float height, float nearZ = 1.f, float farZ = -1.f );
    void SetProjectionPerspective( const float fov, const float aspect, const float farZ = -100.f,
                                   const float nearZ = -1.f );

    void CreateAndSetDepthStencilBuffer();

    //-------------------------------------------------------------------------
    // Camera Accessors
    float GetAspectRatio() const;
    Vec2 GetOutputSize() const { return m_OutputSize; }

    RenderContext* GetCameraContext() const { return m_Owner; }
    Mat44 GetCameraView() const { return m_CameraToClip; }
    Mat44 GetCameraModel() const { return m_CameraModel.GetAsMatrix(); }

    AABB2 GetOrthoView( const float zDist = 0.f ) const;
    OutsideCameraView ArePointsOutside( const std::vector<Vec2>& points, float radius = 0.f );

    Vec3 GetForwardDirection() const;
    Vec3 GetRightDirection() const;
    Vec3 GetLeftDirection() const;
    Vec3 GetUpDirection() const;
    Vec3 GetDownDirection() const;
    Vec3 GetPosition() const { return m_CameraModel.position; }
    Rgba8 GetClearColor( size_t index = 0 ) const;
    float GetClearDepth() const { return m_ClearDepth; }

    //-------------------------------------------------------------------------
    // Mouse Position Conversions
    Vec2 GetNdcSpaceFromClient( const Vec2& clientSpace ) const;
    Vec3 GetWorldSpaceFromClient( const Vec2& clientSpace, const float zDist = 0.f ) const;
    Vec3 GetWorldSpaceFromNdc( const Vec2& ndc, const float zDist = 0.f ) const;
    Vec2 GetWorldSpaceFromMouse( const float zDist = 0.f ) const;

    Vec2 GetScreenSpaceFromWorld( const Vec2& worldPosition ) const;

    //-------------------------------------------------------------------------
    // Camera Bounds Mutators
    void Translate( const Vec3& deltaTranslation );
    void TranslateFPS( const Vec3& deltaTranslation );
    void RotateCamera( const Vec3& rotationAroundAxis );
    void FitCameraInAABB2( const AABB2& bounds );

    void FullyConstrainCamera( bool constrain );
    void ConstrainCameraRotation( bool constrain );
    void WrapCameraRotation( bool axisSpecificWrapping[ 3 ] );
    void ConstrainCameraMovement( bool constrain );
    void SetCameraConstraints( const AABB3& movementConstraints, const AABB3& rotationConstraints );

    //-----------------------------------------------------------------------------
    // Render Functions
    void SetColorTarget( Texture* texture );
    void SetColorTarget( size_t index, Texture* texture );
    Texture* GetColorTarget( size_t index = 0 ) const;
    size_t GetColorTargetCount() const { return m_ColorTargets.size(); }
    Texture* GetDepthTexture() const { return m_DepthStencilBuffer; }
    void SetClearMode( unsigned int clearFlags, const Rgba8& color = Rgba8::BLACK, float depth = 1.f,
                       unsigned int stencil = 0 );
    void SetClearMode( unsigned int clearFlags, const std::vector<Rgba8>& color, float depth = 1.f,
                       unsigned int stencil = 0 );
    void SetClearColor( size_t index, const Rgba8& color );
    bool ShouldClearColor() const;
    bool ShouldClearDepth() const;
    CameraDataUBO GetCameraData() const;

    Transform m_CameraModel;
private:
    Vec2 m_OutputSize;

    bool m_IsRotationallyConstrained = false;
    bool m_LoopRotation[ 3 ] = {false};
    bool m_IsPositionallyConstrained = false;
    AABB3 m_PositionConstraints;
    AABB3 m_RotationConstraints;

    RenderContext* m_Owner = nullptr;
    std::vector<Texture*> m_ColorTargets;
    std::vector<Rgba8> m_ClearColors;
    Texture* m_DepthStencilBuffer = nullptr;
    RenderBuffer* m_CameraUBO = nullptr;

    Mat44 m_CameraToClip;

    unsigned int m_ClearMode = 0;
    float m_ClearDepth = 1.f;
};
