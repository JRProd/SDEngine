#include "Camera.hpp"


#include "Engine/Core/Engine.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"

#include "Engine/Renderer/Texture.hpp"

#include "RenderContext.hpp"

// Should only be visible to this cpp
static OutsideCameraView g_OutsideViewDefault = OUTSIDE_VIEW_RIGHT_COMPLETELY | OUTSIDE_VIEW_TOP_COMPLETELY | OUTSIDE_VIEW_LEFT_COMPLETELY | OUTISDE_VIEW_BOTTOM_COMPLETELY;
static OutsideCameraView g_OutsideViewRemoveRightCompletely = 0xFF ^ OUTSIDE_VIEW_RIGHT_COMPLETELY;
static OutsideCameraView g_OutsideViewRemoveTopCompletely = 0xFF ^ OUTSIDE_VIEW_TOP_COMPLETELY;
static OutsideCameraView g_OutsideViewRemoveLeftCompletely = 0xFF ^ OUTSIDE_VIEW_LEFT_COMPLETELY;
static OutsideCameraView g_OutsideViewRemoveBottomCompletely = 0xFF ^ OUTISDE_VIEW_BOTTOM_COMPLETELY;

//-----------------------------------------------------------------------------
Camera::Camera( RenderContext* owner )
    : m_Owner( owner )
{
}

//-----------------------------------------------------------------------------
Camera::~Camera()
{
    if( m_DepthStencilBuffer != nullptr )
    {
        delete m_DepthStencilBuffer;
        m_DepthStencilBuffer = nullptr;
    }
}

void Camera::SetOutputSize( const Vec2& outputSize )
{
    m_OutputSize = outputSize;
}

void Camera::SetCameraPosition( const Vec3& cameraPosition )
{
    m_CameraModel.position = cameraPosition;
}

void Camera::SetProjectionMatrix( const Mat44& matrix )
{
    m_CameraToClip = matrix;

    const Vec3 bottomLeft = GetWorldSpaceFromClient( Vec2::ZERO );
    const Vec3 topRight = GetWorldSpaceFromClient( Vec2::ONE );

    m_OutputSize = Vec2( topRight.x - bottomLeft.x, topRight.y - bottomLeft.y );
}

void Camera::SetProjectionOrthographic( const AABB2& bounds, const float nearZ, const float farZ )
{
    m_OutputSize = bounds.GetDimensions();
    m_CameraToClip = Mat44::CreateOrthographicProjection( Vec3( bounds.mins, nearZ ), Vec3( bounds.maxs, farZ ) );
}

void Camera::SetProjectionOrthographic( const float height, const float nearZ, const float farZ )
{
    const float aspect = GetAspectRatio();
    GUARANTEE_OR_DIE( !isnan( aspect ), "Camera aspect ratio is undefined. Define aspect ratio before assuming width from height." );

    const float halfHeight = height * .5f;
    const float halfWidth = halfHeight * GetAspectRatio();

    m_OutputSize = Vec2( halfWidth * 2.f, height );
    m_CameraToClip = Mat44::CreateOrthographicProjection( Vec3( -halfWidth, -halfHeight, nearZ ),
                                                          Vec3( halfWidth, halfHeight, farZ ) );
}
void Camera::SetProjectionOrthographicXYPlane( const AABB2& bounds, float nearZ, float farZ )
{
    SetProjectionOrthographic( bounds, nearZ, farZ );
    m_CameraToClip.PushMatrix( Engine::GetCanonicalTransform() );

}
void Camera::SetProjectionOrthographicXYPlane( float height, float nearZ, float farZ )
{
    SetProjectionOrthographic( height, nearZ, farZ );
    m_CameraToClip.PushMatrix( Engine::GetCanonicalTransform() );
}

void Camera::SetProjectionPerspective( const float fov, const float aspect, const float farZ, const float nearZ )
{
    m_CameraToClip = Mat44::CreatePerspectiveProjection( fov, aspect, farZ, nearZ );

    // Determine Output Size
    const Vec3 bottomLeft = GetWorldSpaceFromClient( Vec2::ZERO );
    const Vec3 topRight = GetWorldSpaceFromClient( Vec2::ONE );

    m_OutputSize = Vec2( topRight.x - bottomLeft.x, topRight.y - bottomLeft.y );
}

void Camera::CreateAndSetDepthStencilBuffer()
{
    SetDepthStencilBuffer( Texture::CreateDepthBuffer( m_Owner, m_Owner->GetSwapChainSize() ) );
}

//-----------------------------------------------------------------------------
float Camera::GetAspectRatio() const
{
    return m_OutputSize.x / m_OutputSize.y;
}

AABB2 Camera::GetOrthoView( const float zDist ) const
{
    return AABB2( static_cast<Vec2>(GetWorldSpaceFromNdc( Vec2(-1.f, -1.f), zDist) ),
                  static_cast<Vec2>(GetWorldSpaceFromNdc( Vec2(1.f, 1.f), zDist ) ) );
}

//-----------------------------------------------------------------------------
OutsideCameraView Camera::ArePointsOutside( const std::vector<Vec2>& points, float radius )
{
    // 0xF0 sets the default value for OutsideCameraView. Assume the points are outside completely on
    //  all 4 sides of the camera
    OutsideCameraView outsideView = g_OutsideViewDefault;

    AABB2 orthoBounds = GetOrthoView( 1.f );

    Vec2 bottomRight = orthoBounds.GetBotRight();
    Vec2 topLeft = orthoBounds.GetTopLeft();
    for ( Vec2 point : points )
    {
        if ( point.x > bottomRight.x - radius )
        {
            outsideView |= OUTSIDE_VIEW_RIGHT;
        }
        if ( point.x < bottomRight.x + radius )
        {
            // Removes OUTSIDE_VIEW_RIGHT_COMPLETELY
            outsideView &= g_OutsideViewRemoveRightCompletely;
        }

        if ( point.y > topLeft.y - radius )
        {
            outsideView |= OUTSIDE_VIEW_TOP;
        }
        if ( point.y < topLeft.y + radius )
        {
            // Removes OUTSIDE_VIEW_TOP_COMPLETELY
            outsideView &= g_OutsideViewRemoveTopCompletely;
        }

        if ( point.x < topLeft.x + radius )
        {
            outsideView |= OUTSIDE_VIEW_LEFT;
        }
        if ( point.x > topLeft.x - radius )
        {
            // Removes OUTSIDE_VIEW_LEFT_COMPLETELY
            outsideView &= g_OutsideViewRemoveLeftCompletely;
        }

        if ( point.y < bottomRight.y + radius )
        {
            outsideView |= OUTSIDE_VIEW_BOTTOM;
        }
        if ( point.y > bottomRight.y - radius )
        {
            outsideView &= g_OutsideViewRemoveBottomCompletely;
        }
    }

    return outsideView;
}

Vec3 Camera::GetForwardDirection() const
{
    // const Vec3 canonicalForward = Engine::GetCanonicalTransformInverse().TransformVector( worldForward );
    // return m_CameraModel.GetAsMatrixWithCanonicalTransform().TransformVector( canonicalForward );
    return m_CameraModel.GetAsMatrix().GetIBasis3D();
}

Vec3 Camera::GetRightDirection() const
{
    return -m_CameraModel.GetAsMatrix().GetJBasis3D();
}

Vec3 Camera::GetLeftDirection() const
{
    return m_CameraModel.GetAsMatrix().GetJBasis3D();
}
Vec3 Camera::GetUpDirection() const
{
    return m_CameraModel.GetAsMatrix().GetKBasis3D();
}

Vec3 Camera::GetDownDirection() const
{
    return -m_CameraModel.GetAsMatrix().GetKBasis3D();
}

Rgba8 Camera::GetClearColor( const size_t index) const
{
    size_t actual = Minu( index, m_ClearColors.size() - 1 );
    return m_ClearColors.at(actual);
}

Vec2 Camera::GetNdcSpaceFromClient( const Vec2& clientSpace ) const
{
    return Vec2( Interpolate( -1.f, 1.f, clientSpace.x ),
                 Interpolate( -1.f, 1.f, clientSpace.y ));
}

Vec3 Camera::GetWorldSpaceFromClient( const Vec2& clientSpace, const float zDist ) const
{
    const Vec2 ndcSpace = GetNdcSpaceFromClient( clientSpace );
    return GetWorldSpaceFromNdc( ndcSpace, zDist );
}

//-----------------------------------------------------------------------------
Vec3 Camera::GetWorldSpaceFromNdc( const Vec2& ndc, const float zDist ) const
{
    const Vec3 result = Vec3( ndc.x, ndc.y, zDist );

    Mat44 inverseCameraClip = m_CameraToClip;           // Projection matrix
    inverseCameraClip.Invert();
    Mat44 cameraToWorld = m_CameraModel.GetAsMatrixWithCanonicalTransform();  // Camera model -> inverse view

    cameraToWorld.PushMatrix( inverseCameraClip );

    const Vec4 worldHomogenous = cameraToWorld.TransformHomogeneous(Vec4( result.x, result.y, result.z, 1.f));
    Vec3 worldPos = static_cast<Vec3>(worldHomogenous) / worldHomogenous.w;

    return worldPos;
}

Vec2 Camera::GetWorldSpaceFromMouse( const float zDist ) const
{
    InputSystem* inputSystem = &InputSystem::INSTANCE();
    const Vec3 worldSpace = GetWorldSpaceFromClient( inputSystem->GetMousePositionNormalized(), zDist );

    return static_cast<Vec2>(worldSpace);
}

Vec2 Camera::GetScreenSpaceFromWorld( const Vec2& worldPosition ) const
{
    return GetOrthoView().GetUVAtPoint( worldPosition );
}

void Camera::Translate( const Vec3& deltaTranslation )
{
    m_CameraModel.Translate( deltaTranslation );

    if ( m_IsPositionallyConstrained )
    {
        m_CameraModel.position.x = Clamp( m_CameraModel.position.x, m_PositionConstraints.mins.x, m_PositionConstraints.maxs.x );
        m_CameraModel.position.y = Clamp( m_CameraModel.position.y, m_PositionConstraints.mins.y, m_PositionConstraints.maxs.y );
        m_CameraModel.position.z = Clamp( m_CameraModel.position.z, m_PositionConstraints.mins.z, m_PositionConstraints.maxs.z );
    }
}

void Camera::RotateCamera( const Vec3& rotationAroundAxis )
{
    m_CameraModel.AppendRotationFromAxis( rotationAroundAxis );

    if ( m_IsRotationallyConstrained )
    {
        if( m_LoopRotation[0] )
        {
            m_CameraModel.rotationAroundAxis.x = Wrap( m_CameraModel.rotationAroundAxis.x, m_RotationConstraints.mins.x, m_RotationConstraints.maxs.x );
        }
        else
        {
            m_CameraModel.rotationAroundAxis.x = Clamp( m_CameraModel.rotationAroundAxis.x, m_RotationConstraints.mins.x, m_RotationConstraints.maxs.x );
        }

        if( m_LoopRotation[1] )
        {
            m_CameraModel.rotationAroundAxis.y = Wrap( m_CameraModel.rotationAroundAxis.y, m_RotationConstraints.mins.y, m_RotationConstraints.maxs.y );
        }
        else
        {
            m_CameraModel.rotationAroundAxis.y = Clamp( m_CameraModel.rotationAroundAxis.y, m_RotationConstraints.mins.y, m_RotationConstraints.maxs.y );
        }

        if( m_LoopRotation[2] )
        {
            m_CameraModel.rotationAroundAxis.z = Wrap( m_CameraModel.rotationAroundAxis.z, m_RotationConstraints.mins.z, m_RotationConstraints.maxs.z );
        }
        else
        {
            m_CameraModel.rotationAroundAxis.z = Clamp( m_CameraModel.rotationAroundAxis.z, m_RotationConstraints.mins.z, m_RotationConstraints.maxs.z );
        }
    }
}

void Camera::FitCameraInAABB2( const AABB2& bounds )
{
    AABB2 cameraBounds = GetOrthoView();
    cameraBounds.FitWithinBounds( bounds );

    SetProjectionOrthographic( cameraBounds );
    SetCameraPosition( Vec3::ZERO );
}

void Camera::FullyConstrainCamera( bool constrain )
{
    ConstrainCameraRotation( constrain );
    ConstrainCameraMovement( constrain );
}

void Camera::ConstrainCameraRotation( bool constrain )
{
    m_IsRotationallyConstrained = constrain;
}

void Camera::WrapCameraRotation( bool axisSpecificWrapping[3] )
{
    m_LoopRotation[ 0 ] = axisSpecificWrapping[ 0 ];
    m_LoopRotation[ 1 ] = axisSpecificWrapping[ 1 ];
    m_LoopRotation[ 2 ] = axisSpecificWrapping[ 2 ];
}

void Camera::ConstrainCameraMovement( bool constrain )
{
    m_IsPositionallyConstrained = constrain;
}

void Camera::SetCameraConstraints( const AABB3& movementConstraints, const AABB3& rotationConstraints )
{
    m_PositionConstraints = movementConstraints;
    m_RotationConstraints = rotationConstraints;
}

void Camera::SetColorTarget( Texture* texture )
{
    SetColorTarget( 0, texture );
}

void Camera::SetColorTarget( const size_t index, Texture* texture )
{
    if( index <= m_ColorTargets.size() )
    {
        m_ColorTargets.resize( index + 1 );
    }

    m_ColorTargets[ index ] = texture;
}

Texture* Camera::GetColorTarget( const size_t index ) const
{
    if( m_ColorTargets.empty() )
    {
        return nullptr;
    }
    if( index >= m_ColorTargets.size() )
    {
        return nullptr;
    }

    return m_ColorTargets[ index ];
}

//-----------------------------------------------------------------------------
void Camera::SetClearMode( unsigned int clearFlags, const Rgba8& color, float depth, unsigned int stencil )
{
    UNUSED( stencil );

    m_ClearMode = clearFlags;
    m_ClearDepth = depth;

    if( m_ClearColors.empty() )
    {
        m_ClearColors.push_back( color );
    }
    else
    {
        m_ClearColors[ 0 ] = color;
    }
}

void Camera::SetClearMode( unsigned int clearFlags, const std::vector<Rgba8>& colors, float depth, unsigned int stencil )
{
    UNUSED( stencil );

    m_ClearMode = clearFlags;
    m_ClearDepth = depth;

    m_ClearColors.clear();
    for( const Rgba8& color : colors )
    {
        m_ClearColors.push_back( color );
    }
}

void Camera::SetClearColor( const size_t index, const Rgba8& color )
{
    if( index > m_ClearColors.size() )
    {
        m_ClearColors.resize( index + 1 );
    }

    m_ClearColors[ index ] = color;
}

bool Camera::ShouldClearColor() const
{
    return m_ClearMode & CLEAR_COLOR_BIT;
}

bool Camera::ShouldClearDepth() const
{
    return m_DepthStencilBuffer != nullptr && (m_ClearMode & CLEAR_DEPTH_BIT); 
}


CameraDataUBO Camera::GetCameraData() const
{
    CameraDataUBO cameraData;
    cameraData.cameraToClipTransform = m_CameraToClip;

    Mat44 cameraView = m_CameraModel.GetAsMatrixWithCanonicalTransform();
    cameraView.InvertOrthonormal();
    cameraData.cameraView = cameraView;

    cameraData.position = GetPosition();

    return cameraData;
}
