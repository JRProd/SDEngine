#include "DebugRenderer.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Clock.hpp"

#include "Engine/Core/Transform.hpp"

#include "Camera.hpp"
#include "RenderContext.hpp"
#include "Mesh/MeshUtils.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"

#include <cstdarg>
#include "Engine/Console/Console.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Event/EventSystem.hpp"

#include "Texture.hpp"

static constexpr float ARROW_THICKNESS_TO_LENGTH = .015f;
static constexpr float CELL_HEIGHT_NOT_GIVEN = 0.f;
static Rgba8 I_BASIS_COLOR = Rgba8( 255, 0, 0 );
static Rgba8 J_BASIS_COLOR = Rgba8( 0, 255, 0 );
static Rgba8 K_BASIS_COLOR = Rgba8( 0, 0, 255 );

bool CommandDebugRenderEnable( EventArgs* args)
{
    std::vector<std::string> enabledKeyWords;
    enabledKeyWords.emplace_back( "enabled" );
    const bool shouldEnable = args->GetValue( enabledKeyWords, false );

    if( shouldEnable )
    {
        DebugRenderer::Enable();
    }
    else
    {
        DebugRenderer::Disable();
    }

    return true;
}

bool CommandAddWorldPoint( EventArgs* args)
{
    std::vector<std::string> positionKeyWords;
    positionKeyWords.emplace_back( "position" );
    const Vec3 position = args->GetValue( positionKeyWords, Vec3::ZERO );

    std::vector<std::string> durationKeyWords;
    durationKeyWords.emplace_back( "duration" );
    const float duration = args->GetValue( durationKeyWords, 0.f );

    DebugRenderer::AddWorldPoint( position, Rgba8::WHITE, duration );

    return true;
}

bool CommandAddWorldCubeBounds( EventArgs* args )
{
    std::vector<std::string> minsKeyWords;
    minsKeyWords.emplace_back( "min" );
    const Vec3 mins = args->GetValue( minsKeyWords, Vec3(-.5f) );

    std::vector<std::string> maxsKeyWords;
    maxsKeyWords.emplace_back( "max" );
    const Vec3 maxs = args->GetValue( maxsKeyWords, Vec3(.5f) );

    std::vector<std::string> durationKeyWords;
    durationKeyWords.emplace_back( "duration" );
    const float duration = args->GetValue( durationKeyWords, 0.f );


    Transform transform;
    transform.position = (mins + maxs) * .5f;
    transform.scale = maxs - mins;
    DebugRenderer::AddWorldCubeBounds( transform, Rgba8::YELLOW, duration );

    return true;
}

bool CommandAddWorldSphereBounds( EventArgs* args )
{
    std::vector<std::string> positionKeyWords;
    positionKeyWords.emplace_back( "position" );
    const Vec3 position = args->GetValue( positionKeyWords, Vec3::ZERO );

    std::vector<std::string> radiusKeyWords;
    radiusKeyWords.emplace_back( "radius" );
    const float radius = args->GetValue( radiusKeyWords, 1.f );

    std::vector<std::string> durationKeyWords;
    durationKeyWords.emplace_back( "duration" );
    const float duration = args->GetValue( durationKeyWords, 0.f );

    DebugRenderer::AddWorldSphereBounds( position, Vec3::ZERO, radius, Rgba8::YELLOW, duration);

    return true;
}

bool CommandAddBillboardText( EventArgs* args )
{
    std::vector<std::string> positionKeyWords;
    positionKeyWords.emplace_back( "position" );
    const Vec3 position = args->GetValue( positionKeyWords, Vec3::ZERO );

    const Vec2 align = args->GetValue( "align", Vec2::ALIGN_CENTERED );

    const float duration = args->GetValue( "duration", 0.f );

    std::vector<std::string> textKeyWords;
    textKeyWords.emplace_back( "text" );
    const std::string text = args->GetValue( textKeyWords, "" );

    DebugRenderer::AddBillboardText( position, align, Rgba8::GREEN, duration, text.c_str() );

    return true;
}

bool CommandAddScreenPoint( EventArgs* args )
{
    std::vector<std::string> positionKeyWords;
    positionKeyWords.emplace_back( "position" );
    const Vec2 position = args->GetValue( positionKeyWords, Vec2::ZERO );

    std::vector<std::string> durationKeyWords;
    durationKeyWords.emplace_back( "duration" );
    const float duration = args->GetValue( durationKeyWords, 0.f );

    const float size = args->GetValue( "size", 1.f );

    DebugRenderer::AddScreenPoint( position, size, Rgba8::WHITE, duration );

    return true;
}

bool CommandAddScreenQuad( EventArgs* args )
{
    std::vector<std::string> minsKeyWords;
    minsKeyWords.emplace_back( "min" );
    const Vec2 mins = args->GetValue( minsKeyWords, Vec2( 0.f ) );

    std::vector<std::string> maxsKeyWords;
    maxsKeyWords.emplace_back( "max" );
    const Vec2 maxs = args->GetValue( maxsKeyWords, Vec2( 1.f ) );

    std::vector<std::string> durationKeyWords;
    durationKeyWords.emplace_back( "duration" );
    const float duration = args->GetValue( durationKeyWords, 0.f );

    DebugRenderer::AddScreenQuad( AABB2(mins, maxs), Rgba8::WHITE, duration );

    return true;
}

bool CommandAddScreenText( EventArgs* args )
{
    std::vector<std::string> positionKeyWords;
    positionKeyWords.emplace_back( "position" );
    const Vec2 position = args->GetValue( positionKeyWords, Vec2::ZERO );

    const Vec2 align = args->GetValue( "align", Vec2::ALIGN_BOTTOM_LEFT );

    const float duration = args->GetValue( "duration", 0.f );

    std::vector<std::string> textKeyWords;
    textKeyWords.emplace_back( "text" );
    const std::string text = args->GetValue( textKeyWords, "" );

    DebugRenderer::AddScreenText( Vec4(0.f, 0.f, position.x, position.y), align, Rgba8::GREEN, duration, text.c_str() );

    return true;
}

STATIC void DebugRenderer::Startup()
{
    m_DebugClock = new Clock();

    Command debugRender;
    debugRender.commandName = "Debug_Render";
    debugRender.arguments.push_back( new TypedArgument<bool>("enabled", false ));
    debugRender.description = "Enables or Disables the debug renderer";
    Console::RegisterCommand(debugRender, &CommandDebugRenderEnable);

    Command debugWorldPoint;
    debugWorldPoint.commandName = "Debug_Add_World_Point";
    debugWorldPoint.arguments.push_back( new TypedArgument<Vec3>("position" ));
    debugWorldPoint.arguments.push_back( new TypedArgument<float>("duration", "float" ));
    debugWorldPoint.description = "Draws a world debug point";
    Console::RegisterCommand( debugWorldPoint, &CommandAddWorldPoint);

    Command debugWorldCube;
    debugWorldCube.commandName = "Debug_Add_World_Cube_Bounds";
    debugWorldCube.arguments.push_back( new TypedArgument<Vec3>("mins", "Vec3" ));
    debugWorldCube.arguments.push_back( new TypedArgument<Vec3>("maxs", "Vec3" ));
    debugWorldCube.arguments.push_back( new TypedArgument<float>("duration", "float" ));
    debugWorldCube.description = "Draws a world cube bounds";
    Console::RegisterCommand( debugWorldCube, &CommandAddWorldCubeBounds);

    Command debugWorldSphere;
    debugWorldSphere.commandName = "Debug_Add_World_Sphere_Bounds";
    debugWorldSphere.arguments.push_back( new TypedArgument<Vec3>("position" ));
    debugWorldSphere.arguments.push_back( new TypedArgument<float>("radius" ));
    debugWorldSphere.arguments.push_back( new TypedArgument<float>("duration" ));
    debugWorldSphere.description = "Draws a world sphere bounds";
    Console::RegisterCommand(debugWorldSphere, &CommandAddWorldSphereBounds);

    Command debugBillboardText;
    debugBillboardText.commandName = "Debug_Add_Billboard_Text";
    debugBillboardText.arguments.push_back( new TypedArgument<Vec3>("position", false ));
    debugBillboardText.arguments.push_back( new TypedArgument<std::string>("text", false ));
    debugBillboardText.arguments.push_back( new TypedArgument<Vec2>("align", true, false ));
    debugBillboardText.arguments.push_back( new TypedArgument<float>("duration", true, false ));
    debugBillboardText.description = "Draws billboard text at a world location";
    Console::RegisterCommand(debugBillboardText, &CommandAddBillboardText);

    Command debugScreenPoint;
    debugScreenPoint.commandName = "Debug_Add_Screen_Point";
    debugScreenPoint.arguments.push_back( new TypedArgument<Vec2>("position" ));
    debugScreenPoint.arguments.push_back( new TypedArgument<float>("duration" ));
    debugScreenPoint.arguments.push_back( new TypedArgument<float>("size", true, false ));
    debugScreenPoint.description = "Draws a screen debug point";
    Console::RegisterCommand( debugScreenPoint, &CommandAddScreenPoint);

    Command debugScreenQuad;
    debugScreenQuad.commandName = "Debug_Add_Screen_Quad";
    debugScreenQuad.arguments.push_back( new TypedArgument<Vec2>("mins" ));
    debugScreenQuad.arguments.push_back( new TypedArgument<Vec2>("maxs" ));
    debugScreenQuad.arguments.push_back( new TypedArgument<float>("duration" ));
    debugScreenQuad.description = "Draws a screen quad";
    Console::RegisterCommand( debugScreenQuad, &CommandAddScreenQuad);

    Command debugScreenText;
    debugScreenText.commandName = "Debug_Add_Screen_Text";
    debugScreenText.arguments.push_back( new TypedArgument<Vec2>("position", false ));
    debugScreenText.arguments.push_back( new TypedArgument<std::string>("text",  false ));
    debugScreenText.arguments.push_back( new TypedArgument<Vec2>("align", true, false ));
    debugScreenText.arguments.push_back( new TypedArgument<float>("duration", true, false ));
    debugScreenText.description = "Draws screen text at a pixel location";
    Console::RegisterCommand(debugScreenText, &CommandAddScreenText);
}

STATIC void DebugRenderer::Shutdown()
{
    delete m_Cube;
    delete m_Sphere;
}

void DebugRenderer::Enable()
{
    m_IsEnabled = true;
}

void DebugRenderer::Disable()
{
    m_IsEnabled = false;
}

STATIC void DebugRenderer::Clear()
{
    m_DebugObjects[ RENDER_ALWAYS ].clear();
    m_DebugObjects[ RENDER_USE_DEPTH ].clear();
    m_DebugObjects[ RENDER_X_RAY ].clear();
}

STATIC void DebugRenderer::BeginFrame()
{
}

STATIC void DebugRenderer::RenderToCamera( Camera& camera )
{
    if ( !m_IsEnabled ) { return; }

    m_CurrentCamera = &camera;
    if ( !m_IsFinalized ) { Finalize(); }

    m_CurrentCamera->GetCameraContext()->SetModelUBO();
    m_CurrentCamera->GetCameraContext()->BindTexture( nullptr );

    RenderAlwaysObjects();
    RenderDepthObjects();
    RenderXRayObjects();
    m_CurrentCamera->GetCameraContext()->BindShader( "DEFAULT" );

    m_CurrentCamera = nullptr;
}

STATIC void DebugRenderer::RenderToTexture( Texture& texture )
{
    if ( !m_IsEnabled ) { return; }

    RenderContext* ctx = texture.GetOwner();
    Shader* shaderState = new Shader( nullptr );
    ctx->SaveShaderState( shaderState );

    Camera camera( ctx );
    camera.SetColorTarget( &texture );

    // Create orthographic project
    const Vec2 textureSize = static_cast<const Vec2>(texture.GetTextureSize());
    camera.SetProjectionOrthographicXYPlane( AABB2( Vec2::ZERO, textureSize ), 10000, -10000 );

    camera.CreateAndSetDepthStencilBuffer();
    camera.SetClearMode( CLEAR_DEPTH_BIT, Rgba8::BLACK, 1);

    ctx->BeginCamera( camera );

    m_CurrentCamera = &camera;
    if ( !m_IsFinalized ) { Finalize(); }

    RenderScreenObjects();
    m_CurrentCamera = nullptr;
    ctx->EndCamera( camera );

    ctx->LoadShaderState( shaderState );
    delete shaderState;
}

void DebugRenderer::EndFrame()
{
    for( DebugWorldObject*& object : m_DebugObjects[RENDER_USE_DEPTH])
    {
        if( object != nullptr && object->HasElapsed() )
        {
            object = nullptr;
        }
    }

    for ( DebugWorldObject*& object : m_DebugObjects[ RENDER_ALWAYS ] )
    {
        if ( object != nullptr && object->HasElapsed() )
        {
            object = nullptr;
        }
    }

    for ( DebugWorldObject*& object : m_DebugObjects[ RENDER_X_RAY ] )
    {
        if ( object != nullptr && object->HasElapsed() )
        {
            object = nullptr;
        }
    }

    for( DebugScreenObject*& object: m_DebugScreenObjects )
    {
        if( object != nullptr && object->HasElapsed() )
        {
            object = nullptr;
        }
    }
}

STATIC void DebugRenderer::AddWorldPoint( const Vec3& position, const float size,
                                          const Rgba8& startTint, const Rgba8& endTint,
                                          const float duration, const RenderMode mode )
{
    DebugWorldObject* point = new DebugWorldPoint( position, size, startTint, endTint, duration,
                                                   mode );
    m_DebugObjects[ mode ].push_back( point );
}

STATIC void DebugRenderer::AddWorldPoint( const Vec3& position, const float size, const Rgba8& tint,
                                          const float duration, const RenderMode mode )
{
    AddWorldPoint( position, size, tint, tint, duration, mode );
}

STATIC void DebugRenderer::AddWorldPoint( const Vec3& position, const Rgba8& tint,
                                          const float duration,
                                          const RenderMode mode )
{
    AddWorldPoint( position, 1.f, tint, tint, duration, mode );
}

STATIC void DebugRenderer::AddWorldRay( const LineSeg3D& line, const Rgba8& startPointStartTint,
                                        const Rgba8& startPointEndTint,
                                        const Rgba8& endPointStartTint,
                                        const Rgba8& endPointEndTint, const float thickness,
                                        const float duration, const RenderMode mode )
{
    static Rgba8 constBufferLineColor[ 4 ];
    constBufferLineColor[ 0 ] = startPointStartTint;
    constBufferLineColor[ 1 ] = startPointEndTint;
    constBufferLineColor[ 2 ] = endPointStartTint;
    constBufferLineColor[ 3 ] = endPointEndTint;

    DebugWorldObject* ray = new DebugWorldRay( line, thickness, constBufferLineColor, duration,
                                               mode );
    m_DebugObjects[ mode ].push_back( ray );
}

STATIC void DebugRenderer::AddWorldRay( const LineSeg3D& line, const Rgba8& startPointTint,
                                        const Rgba8& endPointTint, const float thickness,
                                        const float duration,
                                        const RenderMode mode )
{
    AddWorldRay( line, startPointTint, startPointTint, endPointTint,
                 endPointTint, thickness, duration, mode );
}

STATIC void DebugRenderer::AddWorldRay( const LineSeg3D& line, const Rgba8& tint,
                                        const float thickness,
                                        const float duration, const RenderMode mode )
{
    AddWorldRay( line, tint, tint, tint, tint, thickness, duration, mode );
}

STATIC void DebugRenderer::AddWorldRayTimeColorDecay( const LineSeg3D& line,
                                                      const Rgba8& startTimeTint,
                                                      const Rgba8& endTimeTint,
                                                      const float duration,
                                                      const float thickness, const RenderMode mode )
{
    AddWorldRay( line, startTimeTint, endTimeTint, startTimeTint, endTimeTint, thickness, duration,
                 mode );
}

STATIC void DebugRenderer::AddWorldArrow( const LineSeg3D& line, const Rgba8& startPointStartTint,
                                          const Rgba8& startPointEndTint,
                                          const Rgba8& endPointStartTint,
                                          const Rgba8& endPointEndTint, float duration,
                                          RenderMode mode )
{
    static Rgba8 constBufferArrowColor[ 4 ];
    constBufferArrowColor[ 0 ] = startPointStartTint;
    constBufferArrowColor[ 1 ] = startPointEndTint;
    constBufferArrowColor[ 2 ] = endPointStartTint;
    constBufferArrowColor[ 3 ] = endPointEndTint;

    DebugWorldObject* arrow = new DebugWorldArrow( line, constBufferArrowColor, duration, mode );
    m_DebugObjects[ mode ].push_back( arrow );
}

STATIC void DebugRenderer::AddWorldArrow( const LineSeg3D& line, const Rgba8& startPointTint,
                                          const Rgba8& endPointTint, float duration,
                                          RenderMode mode )
{
    AddWorldArrow( line, startPointTint, startPointTint, endPointTint, endPointTint, duration,
                   mode );
}

STATIC void DebugRenderer::AddWorldArrow( const LineSeg3D& line, const Rgba8& tint, float duration,
                                          RenderMode mode )
{
    AddWorldArrow( line, tint, tint, tint, tint, duration, mode );
}

STATIC void DebugRenderer::AddWorldArrowTimeColorDelay( const LineSeg3D& line,
                                                        const Rgba8& startTimeTint,
                                                        const Rgba8& endTimeTint, float duration,
                                                        RenderMode mode )
{
    AddWorldArrow( line, startTimeTint, endTimeTint, startTimeTint, endTimeTint, duration, mode );
}

void DebugRenderer::AddWorldPlane( const Plane2D& plane, const Rgba8& startPlaneTint, const Rgba8& endPlaneTint, const Rgba8& startArrowTint, const Rgba8& endArrowTint, float duration, RenderMode mode )
{
    static Rgba8 constBufferPlaneColor[ 4 ];
    constBufferPlaneColor[ 0 ] = startPlaneTint;
    constBufferPlaneColor[ 1 ] = endPlaneTint;
    constBufferPlaneColor[ 2 ] = startArrowTint;
    constBufferPlaneColor[ 3 ] = endArrowTint;
    DebugWorldObject* planeObject = new DebugWorldPlane( plane, constBufferPlaneColor, duration, mode );
    m_DebugObjects[ mode ].push_back( planeObject );
}

void DebugRenderer::AddWorldPlane( const Plane2D& plane, const Rgba8& planeTint, const Rgba8& arrowTint, float duration, RenderMode mode )
{
    AddWorldPlane( plane, planeTint, planeTint, arrowTint, arrowTint, duration, mode );
}

void DebugRenderer::AddWorldPlane( const Plane2D& plane, const Rgba8& tint, float duration, RenderMode mode )
{
    AddWorldPlane( plane, tint, tint, duration, mode );
}

void DebugRenderer::AddWorldPlane( const Plane2D& plane, float duration, RenderMode mode )
{
    AddWorldPlane( plane, Rgba8::BLACK, Rgba8::RED, duration, mode );
}

void DebugRenderer::AddWorldQuad( const Mat44& basis, const Vec2& ijSize, Texture* texture, const AABB2& uvs, const Rgba8& startTint, const Rgba8& endTint, float duration, RenderMode mode )
{
    DebugWorldObject* quad = new DebugWorldQuad( basis, ijSize, texture, uvs, startTint, endTint, duration, mode );
    m_DebugObjects[ mode ].push_back( quad );
}

void DebugRenderer::AddWorldCubeBounds( const Transform& transform, const Rgba8& startTint,
                                        const Rgba8& endTint, const float duration,
                                        const RenderMode mode )
{
    AddWorldMesh( m_Cube, transform, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldCubeBounds( const Transform& transform, const Rgba8& tint,
                                        const float duration, const RenderMode mode )
{
    AddWorldCubeBounds( transform, tint, tint, duration, mode );
}

void DebugRenderer::AddWorldCubeBounds( const Vec3& position, const Vec3& rotation,
                                        const Vec3& scale, const Rgba8& startTint,
                                        const Rgba8& endTint, const float duration,
                                        const RenderMode mode )
{
    const Transform transform( position, rotation, scale );
    AddWorldCubeBounds( transform, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldCubeBounds( const Vec3& position, const Vec3& rotation,
                                        const Vec3& scale, const Rgba8& tint, const float duration,
                                        const RenderMode mode )
{
    const Transform transform( position, scale, rotation );
    AddWorldCubeBounds( transform, tint, duration, mode );
}

void DebugRenderer::AddWorldCubeBounds( const Vec3& position, const Vec3& rotation,
                                        const Rgba8& startTint, const Rgba8& endTint,
                                        const float duration, const RenderMode mode )
{
    AddWorldCubeBounds( position, rotation, Vec3::ONE, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldCubeBounds( const Vec3& position, const Rgba8& startTint,
                                        const Rgba8& endTint, const float duration,
                                        const RenderMode mode )
{
    AddWorldCubeBounds( position, Vec3::ZERO, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Transform& transform, const Rgba8& startTint,
                                          const Rgba8& endTint, float duration, RenderMode mode )
{
    AddWorldMesh( m_Sphere, transform, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Transform& transform, const Rgba8& tint,
                                          float duration, RenderMode mode )
{
    AddWorldSphereBounds( transform, tint, tint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Vec3& position, const Vec3& rotation,
                                          const Vec3& scale, const Rgba8& startTint,
                                          const Rgba8& endTint, float duration, RenderMode mode )
{
    const Transform transform( position, rotation, scale );
    AddWorldSphereBounds( transform, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Vec3& position, const Vec3& rotation, float radius,
                                          const Rgba8& startTint, const Rgba8& endTint,
                                          float duration, RenderMode mode )
{
    AddWorldSphereBounds( position, rotation, Vec3( radius ), startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Vec3& position, const Vec3& rotation,
                                          const Vec3& scale, const Rgba8& tint, float duration,
                                          RenderMode mode )
{
    const Transform transform( position, scale, rotation );
    AddWorldSphereBounds( transform, tint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Vec3& position, const Vec3& rotation, float radius,
                                          const Rgba8& tint, float duration, RenderMode mode )
{
    AddWorldSphereBounds( position, rotation, Vec3( radius ), tint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Vec3& position, const Vec3& rotation,
                                          const Rgba8& startTint, const Rgba8& endTint,
                                          float duration, RenderMode mode )
{
    AddWorldSphereBounds( position, rotation, Vec3::ONE, startTint, endTint, duration, mode );
}

void DebugRenderer::AddWorldSphereBounds( const Vec3& position, const Rgba8& startTint,
                                          const Rgba8& endTint, float duration, RenderMode mode )
{
    AddWorldSphereBounds( position, Vec3::ZERO, startTint, endTint, duration, mode );
}

STATIC void DebugRenderer::AddWorldBasis( const Mat44& basis, const Rgba8& startTint,
                                          const Rgba8& endTint, const float duration,
                                          const RenderMode mode )
{
    DebugWorldObject* worldBasis = new DebugWorldBasis( basis, startTint, endTint, duration, mode );
    m_DebugObjects[ mode ].push_back( worldBasis );
}

STATIC void DebugRenderer::AddWorldBasis( const Mat44& basis, const float duration,
                                          const RenderMode mode )
{
    AddWorldBasis( basis, Rgba8::WHITE, Rgba8::WHITE, duration, mode );
}

STATIC void DebugRenderer::AddWorldText( const Mat44& basis, const Vec2& pivot,
                                         const Rgba8& startTint, const Rgba8& endTint,
                                         const float duration, const RenderMode mode,
                                         const char* text )
{
    DebugWorldObject* worldText = new DebugWorldText( text, pivot, m_WorldFontHeight,
                                                      m_WorldFontAspect, basis, startTint, endTint,
                                                      duration, mode );
    m_DebugObjects[ mode ].push_back( worldText );
}

STATIC void DebugRenderer::AddWorldText( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                                         const float duration, const char* text )
{
    AddWorldText( basis, pivot, tint, tint, duration, RENDER_USE_DEPTH, text );
}

STATIC void DebugRenderer::AddWorldText( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                                         const char* text )
{
    AddWorldText( basis, pivot, tint, 0.f, text );
}

void DebugRenderer::AddWorldText( const Mat44& basis, const char* text )
{
    AddWorldText( basis, Vec2::ALIGN_CENTERED, Rgba8::WHITE, text );
}

STATIC void DebugRenderer::AddWorldTextf( const Mat44& basis, const Vec2& pivot,
                                          const Rgba8& startTint, const Rgba8& endTint,
                                          const float duration, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddWorldText( basis, pivot, startTint, endTint, duration, RENDER_USE_DEPTH, formatted.c_str() );
}

STATIC void DebugRenderer::AddWorldTextf( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                                          const float duration, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddWorldText( basis, pivot, tint, tint, duration, RENDER_USE_DEPTH, formatted.c_str() );
}

STATIC void DebugRenderer::AddWorldTextf( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                                          const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddWorldText( basis, pivot, tint, tint, 0.f, RENDER_USE_DEPTH, formatted.c_str() );
}

void DebugRenderer::AddWorldTextf( const Mat44& basis, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddWorldText( basis, Vec2::ALIGN_CENTERED, Rgba8::WHITE, Rgba8::WHITE, 0.f, RENDER_USE_DEPTH,
                  formatted.c_str() );
}

STATIC void DebugRenderer::AddBillboardText( const Vec3& origin, const Vec2& pivot,
                                             const Rgba8& startTint, const Rgba8& endTint,
                                             const float duration, const RenderMode mode,
                                             const char* text )
{
    DebugWorldObject* billboardText = new DebugBillboardText(
        text, origin, pivot, m_WorldFontHeight, m_WorldFontAspect, startTint, endTint, duration,
        mode );
    m_DebugObjects[ mode ].push_back( billboardText );
}

STATIC void DebugRenderer::AddBillboardText( const Vec3& origin, const Vec2& pivot,
                                             const Rgba8& tint, const float duration,
                                             const char* text )
{
    AddBillboardText( origin, pivot, tint, tint, duration, RENDER_USE_DEPTH, text );
}

STATIC void DebugRenderer::AddBillboardText( const Vec3& origin, const Vec2& pivot,
                                             const Rgba8& tint, const char* text )
{
    AddBillboardText( origin, pivot, tint, 0.f, text );
}

void DebugRenderer::AddBillboardText( const Vec3& origin, const char* text )
{
    AddBillboardText( origin, Vec2::ALIGN_CENTERED, Rgba8::WHITE, text );
}

STATIC void DebugRenderer::AddBillboardTextf( const Vec3& origin, const Vec2& pivot,
                                              const Rgba8& startTint, const Rgba8& endTint,
                                              const float duration, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddBillboardText( origin, pivot, startTint, endTint, duration, RENDER_USE_DEPTH,
                      formatted.c_str() );
}

STATIC void DebugRenderer::AddBillboardTextf( const Vec3& origin, const Vec2& pivot,
                                              const Rgba8& tint, const float duration,
                                              const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddBillboardText( origin, pivot, tint, tint, duration, RENDER_USE_DEPTH, formatted.c_str() );
}

STATIC void DebugRenderer::AddBillboardTextf( const Vec3& origin, const Vec2& pivot,
                                              const Rgba8& tint, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddBillboardText( origin, pivot, tint, tint, 0.f, RENDER_USE_DEPTH, formatted.c_str() );
}

void DebugRenderer::AddBillboardTextf( const Vec3& origin, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddBillboardText( origin, Vec2::ALIGN_CENTERED, Rgba8::WHITE, Rgba8::WHITE, 0.f,
                      RENDER_USE_DEPTH, formatted.c_str() );
}

void DebugRenderer::AddScreenPoint( const Vec2& position, float size, const Rgba8& startTint,
                                    const Rgba8& endTint, float duration )
{
    DebugScreenObject* point = new DebugScreenPoint( position, size, startTint, endTint, duration );
    m_DebugScreenObjects.push_back( point );
}

void DebugRenderer::AddScreenPoint( const Vec2& position, float size, const Rgba8& tint,
                                    float duration )
{
    AddScreenPoint( position, size, tint, tint, duration );
}

void DebugRenderer::AddScreenPoint( const Vec2& position, const Rgba8& tint, float duration )
{
    AddScreenPoint( position, 1.f, tint, tint, duration );
}

void DebugRenderer::AddScreenLine( const LineSeg2D& lineSeg, const Rgba8& startPointStartTint,
                                   const Rgba8& startPointEndTint,
                                   const Rgba8& endPointStartTint,
                                   const Rgba8& endPointEndTint, float thickness,
                                   float duration )
{
    static Rgba8 constBufferLineTint[ 4 ];
    constBufferLineTint[ 0 ] = startPointStartTint;
    constBufferLineTint[ 1 ] = startPointEndTint;
    constBufferLineTint[ 2 ] = endPointStartTint;
    constBufferLineTint[ 3 ] = endPointEndTint;

    DebugScreenObject* line = new DebugScreenLine( lineSeg, thickness, constBufferLineTint,
                                                   duration );
    m_DebugScreenObjects.push_back( line );
}

void DebugRenderer::AddScreenLine( const LineSeg2D& lineSeg, const Rgba8& startPointTint,
                                   const Rgba8& endPointTint, const float thickness,
                                   const float duration )
{
    AddScreenLine( lineSeg, startPointTint, startPointTint, endPointTint, endPointTint,
                   thickness, duration );
}

void DebugRenderer::AddScreenLine( const LineSeg2D& lineSeg, const Rgba8& tint,
                                   const float thickness, const float duration )
{
    AddScreenLine( lineSeg, tint, tint, thickness, duration );
}

void DebugRenderer::AddScreenLine( const LineSeg2D& lineSeg, const float thickness,
                                   const Rgba8& tint, const float duration )
{
    AddScreenLine( lineSeg, tint, thickness, duration );
}

void DebugRenderer::AddScreenLineTimeColorDelay( const LineSeg2D& lineSeg,
                                                 const Rgba8& startTimeTint,
                                                 const Rgba8& endTimeTint,
                                                 const float thickness, const float duration )
{
    AddScreenLine( lineSeg, startTimeTint, endTimeTint, startTimeTint, endTimeTint, thickness,
                   duration );
}

void DebugRenderer::AddScreenArrow( const LineSeg2D& lineSeg, const Rgba8& startPointStartTint,
                                    const Rgba8& startPointEndTint,
                                    const Rgba8& endPointStartTint,
                                    const Rgba8& endPointEndTint, const float duration )
{
    static Rgba8 constBufferArrowTint[ 4 ];
    constBufferArrowTint[ 0 ] = startPointStartTint;
    constBufferArrowTint[ 1 ] = startPointEndTint;
    constBufferArrowTint[ 2 ] = endPointStartTint;
    constBufferArrowTint[ 3 ] = endPointEndTint;

    DebugScreenObject* arrow = new DebugScreenArrow( lineSeg, constBufferArrowTint, duration );
    m_DebugScreenObjects.push_back( arrow );
}

void DebugRenderer::AddScreenArrow( const LineSeg2D& lineSeg, const Rgba8& startPointTint,
                                    const Rgba8& endPointTint, const float duration )
{
    AddScreenArrow( lineSeg, startPointTint, startPointTint, endPointTint, endPointTint,
                    duration );
}

void DebugRenderer::AddScreenArrow( const LineSeg2D& lineSeg, const Rgba8& tint,
                                    const float duration )
{
    AddScreenArrow( lineSeg, tint, tint, duration );
}

void DebugRenderer::AddScreenArrowTimeColorDelay( const LineSeg2D& lineSeg,
                                                  const Rgba8& startTimeTint,
                                                  const Rgba8& endTimeTint,
                                                  const float duration )
{
    AddScreenArrow( lineSeg, startTimeTint, endTimeTint, startTimeTint, endTimeTint,
                    duration );
}

void DebugRenderer::AddScreenQuad( const AABB2& bounds, Texture* texture, const AABB2& uvs,
                                   const Rgba8& startTint, const Rgba8& endTint,
                                   const float duration )
{
    DebugScreenObject* quad = new DebugScreenQuad( bounds, texture, uvs, startTint, endTint,
                                                   duration );
    m_DebugScreenObjects.push_back( quad );
}

void DebugRenderer::AddScreenQuad( const AABB2& bounds, Texture* texture, const AABB2& uvs,
                                   const Rgba8& tint, const float duration )
{
    AddScreenQuad( bounds, texture, uvs, tint, tint, duration );
}

void DebugRenderer::AddScreenQuad( const AABB2& bounds, Texture* texture,
                                   const Rgba8& startTint, const Rgba8& endTint,
                                   const float duration )
{
    AddScreenQuad( bounds, texture, AABB2::UNIT_BOX, startTint, endTint, duration );
}

void DebugRenderer::AddScreenQuad( const AABB2& bounds, Texture* texture, const Rgba8& tint,
                                   const float duration )
{
    AddScreenQuad( bounds, texture, tint, tint, duration );
}

void DebugRenderer::AddScreenQuad( const AABB2& bounds, const Rgba8& startTint,
                                   const Rgba8& endTint, const float duration )
{
    AddScreenQuad( bounds, nullptr, startTint, endTint, duration );
}

void DebugRenderer::AddScreenQuad( const AABB2& bounds, const Rgba8& tint, const float duration )
{
    AddScreenQuad( bounds, tint, tint, duration );
}

void DebugRenderer::AddScreenText( const Vec4& originAndOffset, const Vec2& pivot,
                                   const float cellHeight, const float cellAspect,
                                   const Rgba8& startTint,
                                   const Rgba8& endTint, const float duration, const char* text )
{
    DebugScreenObject* screenText = new DebugScreenTextObject(
        text, originAndOffset, pivot, cellHeight, cellAspect, startTint, endTint, duration );
    m_DebugScreenObjects.push_back( screenText );
}

void DebugRenderer::AddScreenText( const Vec4& originAndOffset, const Vec2& pivot,
                                   const float cellHeight, const float cellAspect,
                                   const Rgba8& tint,
                                   const float duration, const char* text )
{
    AddScreenText( originAndOffset, pivot, cellHeight, cellAspect, tint, tint, duration, text );
}

void DebugRenderer::AddScreenText( const Vec4& originAndOffset, const Vec2& pivot,
                                   const Rgba8& tint, const float duration, const char* text )
{
    AddScreenText( originAndOffset, pivot, CELL_HEIGHT_NOT_GIVEN, m_ScreenFontAspect, tint, duration,
                   text );
}

void DebugRenderer::AddScreenText( const Vec4& originAnOffset, const char* text )
{
    AddScreenText( originAnOffset, Vec2::ALIGN_CENTERED, Rgba8::WHITE, 0.f, text );
}

void DebugRenderer::AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot,
                                    const float cellHeight, const float cellAspect,
                                    const Rgba8& startTint,
                                    const Rgba8& endTint, const float duration, const char* format,
                                    ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddScreenText( originAndOffset, pivot, cellHeight, cellAspect, startTint, endTint, duration,
                   formatted.c_str() );
}

void DebugRenderer::AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot,
                                    const float cellHeight, const float cellAspect,
                                    const Rgba8& tint,
                                    const float duration, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddScreenText( originAndOffset, pivot, cellHeight, cellAspect, tint, tint, duration,
                   formatted.c_str() );
}

void DebugRenderer::AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot,
                                    const Rgba8& tint, const float duration, const char* format,
                                    ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddScreenText( originAndOffset, pivot, CELL_HEIGHT_NOT_GIVEN, m_ScreenFontAspect, tint, tint,
                   duration, formatted.c_str() );
}

void DebugRenderer::AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddScreenText( originAndOffset, pivot, CELL_HEIGHT_NOT_GIVEN, m_ScreenFontAspect, Rgba8::WHITE, 0.f, formatted.c_str() );
}

void DebugRenderer::AddScreenTextf( const Vec4& originAndOffset, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    const std::string formatted = Stringv( format, args );

    AddScreenText( originAndOffset, Vec2::ALIGN_CENTERED, CELL_HEIGHT_NOT_GIVEN, m_ScreenFontAspect,
                   Rgba8::WHITE, Rgba8::WHITE, 0.f, formatted.c_str() );
}

void DebugRenderer::AddScreenBasis( const Vec4& originAndOffset, const Mat44& basis, const Rgba8& startTint, const Rgba8& endTint, const float duration )
{
    DebugScreenObject* screenBasis = new DebugScreenBasis( originAndOffset, basis, 100.f, startTint, endTint, duration );
    m_DebugScreenObjects.push_back( screenBasis );
}

void DebugRenderer::AddScreenBasis( const Vec4& originAndOffset, const Rgba8& startTint, const Rgba8& endTint, const float duration )
{
    AddScreenBasis( originAndOffset, Mat44::IDENTITY, startTint, endTint, duration );
}

void DebugRenderer::AddScreenBasis( const Vec4& originAndOffset, const Mat44& basis, const Rgba8& tint, const float duration )
{
    AddScreenBasis( originAndOffset, basis, tint, tint, duration );
}

void DebugRenderer::AddWorldMesh( GPUMesh*& mesh, const Transform& transform,
                                  const Rgba8& startTint,
                                  const Rgba8& endTint, const float duration, const RenderMode mode )
{
    DebugWorldObject* meshObject = new DebugWorldMesh( mesh, transform.GetAsMatrix(), startTint,
                                                       endTint, duration, mode );
    m_DebugObjects[ mode ].push_back( meshObject );
}

STATIC void DebugRenderer::RenderAlwaysObjects()
{
    RenderContext* cameraContext = m_CurrentCamera->GetCameraContext();
    cameraContext->BindShader( nullptr );

    for ( DebugWorldObject*& alwaysObject : m_DebugObjects[ RENDER_ALWAYS ] )
    {
        if ( alwaysObject == nullptr ) { continue; }

        alwaysObject->RenderObject( *cameraContext, *m_CurrentCamera );
    }
}

STATIC void DebugRenderer::RenderDepthObjects()
{
    RenderContext* cameraContext = m_CurrentCamera->GetCameraContext();
    cameraContext->BindShader( m_Depth );

    for ( DebugWorldObject*& alwaysObject : m_DebugObjects[ RENDER_USE_DEPTH ] )
    {
        if ( alwaysObject == nullptr ) { continue; }

        alwaysObject->RenderObject( *cameraContext, *m_CurrentCamera );
    }
}

STATIC void DebugRenderer::RenderXRayObjects()
{
    RenderContext* cameraContext = m_CurrentCamera->GetCameraContext();

    cameraContext->BindShader( nullptr );
    for ( DebugWorldObject*& alwaysObject : m_DebugObjects[ RENDER_X_RAY ] )
    {
        if ( alwaysObject == nullptr ) { continue; }

        alwaysObject->RenderObject( *cameraContext, *m_CurrentCamera );
    }

    cameraContext->BindShader( m_XRay );
    for ( DebugWorldObject*& alwaysObject : m_DebugObjects[ RENDER_X_RAY ] )
    {
        if ( alwaysObject == nullptr ) { continue; }

        alwaysObject->RenderObject( *cameraContext, *m_CurrentCamera );
    }
}

void DebugRenderer::RenderScreenObjects()
{
    RenderContext* cameraContext = m_CurrentCamera->GetCameraContext();

    for ( DebugScreenObject*& screenObject : m_DebugScreenObjects )
    {
        if ( screenObject == nullptr ) { continue; }

        screenObject->RenderObject( *cameraContext,
                                    static_cast<IntVec2>(m_CurrentCamera->GetOutputSize()) );

        if( screenObject->HasElapsed() )
        {
            screenObject = nullptr;
        }
    }
}

void DebugRenderer::Finalize()
{
    RenderContext* ctx = m_CurrentCamera->GetCameraContext();

    m_Cube = GPUMesh::CreateCube( ctx, Vec3( .5f, .5f, .5f ) );
    m_Sphere = GPUMesh::CreateSphereUv( ctx, .5f, Vec2( 32, 16 ) );

    m_Depth = new Shader( ctx->CreateOrGetShaderProgramFromFile( "DEFAULT" ) );
    m_Depth->writeDepth = false;
    m_Depth->depthCompare = DepthCompare::LESS_EQUAL;

    m_XRay = new Shader( ctx->CreateOrGetShaderProgramFromFile( "Data/Shaders/default/xray.hlsl" ) );
    m_XRay->writeDepth = false;
    m_XRay->depthCompare = DepthCompare::GREATER;

    m_IsFinalized = true;
}

STATIC Clock* DebugRenderer::m_DebugClock = nullptr;
STATIC GPUMesh* DebugRenderer::m_Cube = nullptr;
STATIC GPUMesh* DebugRenderer::m_Sphere = nullptr;
STATIC std::vector<DebugWorldObject*> DebugRenderer::m_DebugObjects[ RENDER_NUM_MODES ];
STATIC std::vector<DebugScreenObject*> DebugRenderer::m_DebugScreenObjects;
STATIC BitmapFont* DebugRenderer::m_DebugFont = nullptr;
STATIC float DebugRenderer::m_WorldFontHeight = 1.f;
STATIC float DebugRenderer::m_WorldFontAspect = .56f;
STATIC float DebugRenderer::m_ScreenFontHeightPercent = .05f;
STATIC float DebugRenderer::m_ScreenFontAspect = .56f;

STATIC Camera* DebugRenderer::m_CurrentCamera = nullptr;
STATIC Shader* DebugRenderer::m_XRay = nullptr;
STATIC Shader* DebugRenderer::m_Depth= nullptr;
STATIC bool DebugRenderer::m_IsFinalized = false;
STATIC bool DebugRenderer::m_IsEnabled = true;

DebugWorldObject::DebugWorldObject( const float duration, const RenderMode mode )
    : m_Mode( mode )
{
    m_Duration = new Timer();
    m_Duration->Set( DebugRenderer::m_DebugClock, duration );
}

DebugWorldObject::~DebugWorldObject()
{
    delete m_Duration;
    m_Duration = nullptr;
}

bool DebugWorldObject::HasElapsed() const
{
    return m_Duration->HasElapsed();
}

DebugWorldPoint::DebugWorldPoint( const Vec3& position, const float size, const Rgba8& startTint,
                                  const Rgba8& endTint, const float duration,
                                  const RenderMode mode )
    : DebugWorldObject( duration, mode )
      , m_Position( position )
      , m_Size( size )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugWorldPoint::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    std::vector<VertexMaster> visual;
    const Mat44 cameraRotation = currentCamera.GetCameraModel();

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );
    AppendPlaneSegment( visual, m_Position, cameraRotation.GetIBasis3D() * m_Size,
                        cameraRotation.GetJBasis3D() * m_Size, tint );

    ctx.DrawVertexArray( visual );
}

DebugWorldRay::DebugWorldRay( const LineSeg3D& lineSeg, const float thickness,
                              const Rgba8* lineColorArray4,
                              const float duration, const RenderMode mode )
    : DebugWorldObject( duration, mode )
      , m_LineSegment( lineSeg )
      , m_Thickness( thickness )
{
    m_StartPointStartColor = lineColorArray4[ 0 ];
    m_StartPointEndColor = lineColorArray4[ 1 ];
    m_EndPointStartColor = lineColorArray4[ 2 ];
    m_EndPointEndColor = lineColorArray4[ 3 ];
}

void DebugWorldRay::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    UNUSED( currentCamera );

    const Rgba8 startTint = Rgba8::Lerp( m_StartPointStartColor, m_StartPointEndColor,
                                         m_Duration->GetPercentage() );
    const Rgba8 endTint = Rgba8::Lerp( m_EndPointStartColor, m_EndPointEndColor,
                                       m_Duration->GetPercentage() );
    std::vector<VertexMaster> visual;
    AppendRay( visual, m_LineSegment, startTint, endTint, m_Thickness );

    ctx.DrawVertexArray( visual );
}

DebugWorldArrow::DebugWorldArrow( const LineSeg3D& lineSeg, const Rgba8* arrowColorArray4,
                                  const float duration,
                                  const RenderMode mode )
    : DebugWorldObject( duration, mode )
      , m_LineSegment( lineSeg )
{
    m_StartPointStartColor = arrowColorArray4[ 0 ];
    m_StartPointEndColor = arrowColorArray4[ 1 ];
    m_EndPointStartColor = arrowColorArray4[ 2 ];
    m_EndPointEndColor = arrowColorArray4[ 3 ];
}

void DebugWorldArrow::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    UNUSED( currentCamera );

    const Rgba8 startTint = Rgba8::LerpAsHSL( m_StartPointStartColor, m_StartPointEndColor,
                                         m_Duration->GetPercentage() );
    const Rgba8 endTint = Rgba8::LerpAsHSL( m_EndPointStartColor, m_EndPointEndColor,
                                       m_Duration->GetPercentage() );

    const float thickness = m_LineSegment.GetLength() * ARROW_THICKNESS_TO_LENGTH;

    std::vector<VertexMaster> visual;
    std::vector<unsigned int> index;
    AppendArrow3D( visual, index, m_LineSegment, startTint, endTint, thickness );

    ctx.DrawIndexed( visual, index );
}

DebugWorldBasis::DebugWorldBasis( const Mat44& basis, const Rgba8& startTint,
                                  const Rgba8& endTint, const float duration,
                                  const RenderMode mode )
    : DebugWorldObject( duration, mode )
      , m_Basis( basis )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugWorldBasis::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    UNUSED( currentCamera );

    const Rgba8 lerpAmount = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );
    const Rgba8 iBasisColor = I_BASIS_COLOR * lerpAmount;
    const Rgba8 jBasisColor = J_BASIS_COLOR * lerpAmount;
    const Rgba8 kBasisColor = K_BASIS_COLOR * lerpAmount;

    const Vec3 position = m_Basis.GetTranslation3D();
    const float thickness = ARROW_THICKNESS_TO_LENGTH * 2.f;

    std::vector<VertexMaster> visual;
    std::vector<unsigned int> index;
    AppendArrow3D( visual, index, LineSeg3D( position, position + m_Basis.GetIBasis3D() ),
                   iBasisColor, thickness );
    AppendArrow3D( visual, index, LineSeg3D( position, position + m_Basis.GetJBasis3D() ),
                   jBasisColor, thickness );
    AppendArrow3D( visual, index, LineSeg3D( position, position + m_Basis.GetKBasis3D() ),
                   kBasisColor, thickness );

    ctx.DrawIndexed( visual, index );
}

STATIC Shader* DebugWorldMesh::m_WireframeShader = nullptr;
STATIC bool DebugWorldMesh::m_Finalized = false;

DebugWorldMesh::DebugWorldMesh( GPUMesh*& mesh, const Mat44& transform, const Rgba8& startTint,
                                const Rgba8& endTint, const float duration, const RenderMode mode )
    : DebugWorldBasis( transform, startTint, endTint, duration, mode )
      , m_Mesh( mesh )
{
}

void DebugWorldMesh::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    UNUSED( currentCamera );

    if( !m_Finalized )
    {
        m_WireframeShader = new Shader( ctx.CreateOrGetShaderProgramFromFile( "DEFAULT" ) );
        m_WireframeShader->cullMode = CullMode::NONE;
        m_WireframeShader->fillMode = FillMode::LINE;

        m_Finalized = true;
    }

    Shader* lastShader = new Shader( nullptr );
    ctx.SaveShaderState( lastShader );

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );

    ctx.BindShader( m_WireframeShader );
    ctx.SetModelUBO( m_Basis, tint );
    ctx.DrawMesh( m_Mesh );

    ctx.SetModelUBO();

    ctx.LoadShaderState( lastShader );
    delete lastShader;
}

DebugWorldText::DebugWorldText( const std::string& text, const Vec2& pivot, const float cellHeight,
                                const float cellAspect, const Mat44& basis, const Rgba8& startTint,
                                const Rgba8& endTint, const float duration, const RenderMode mode )
    : DebugWorldBasis( basis, startTint, endTint, duration, mode )
      , m_Text( text )
      , m_Pivot( pivot )
      , m_CellHeight( cellHeight )
      , m_CellAspect( cellAspect )
{
}

void DebugWorldText::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    UNUSED( currentCamera );

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );

    std::vector<VertexMaster> visual;
    DebugRenderer::m_DebugFont->AddVertsForText3D( visual, m_Text, m_Basis, m_Pivot, m_CellHeight,
                                                   tint, m_CellAspect );

    ctx.BindTexture( DebugRenderer::m_DebugFont->GetTexture() );
    ctx.DrawVertexArray( visual );
    ctx.BindTexture( nullptr );
}

DebugBillboardText::DebugBillboardText( const std::string& text, const Vec3& origin,
                                        const Vec2& pivot, const float cellHeight,
                                        const float cellAspect,
                                        const Rgba8& startTint, const Rgba8& endTint,
                                        const float duration, const RenderMode mode )
    : DebugWorldObject( duration, mode )
      , m_Text( text )
      , m_Origin( origin )
      , m_Pivot( pivot )
      , m_CellHeight( cellHeight )
      , m_CellAspect( cellAspect )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugBillboardText::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    Mat44 textBasis = currentCamera.GetCameraModel();
    textBasis.SetTranslation( m_Origin );

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );

    std::vector<VertexMaster> visual;
    DebugRenderer::m_DebugFont->AddVertsForText3D( visual, m_Text, textBasis, m_Pivot, m_CellHeight,
                                                   tint, m_CellAspect );

    ctx.BindTexture( DebugRenderer::m_DebugFont->GetTexture() );
    ctx.DrawVertexArray( visual );
    ctx.BindTexture( nullptr );
}

DebugScreenObject::DebugScreenObject( const float duration )
{
    m_Duration = new Timer();
    m_Duration->Set( DebugRenderer::m_DebugClock, duration );
}

bool DebugScreenObject::HasElapsed() const
{
    return m_Duration->HasElapsed();
}

DebugScreenPoint::DebugScreenPoint( const Vec2& position, const float size, const Rgba8& startTint,
                                    const Rgba8& endTint, const float duration )
    : DebugScreenObject( duration )
      , m_Position( position )
      , m_Size( size )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugScreenPoint::RenderObject( RenderContext& ctx, const IntVec2& screenSize )
{
    UNUSED( screenSize );

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );

    std::vector<VertexMaster> visual;
    AppendPlaneSegment( visual, Vec3( m_Position ), Vec3::I * m_Size, Vec3::J * m_Size, tint );

    ctx.DrawVertexArray( visual );
}

DebugScreenLine::DebugScreenLine( const LineSeg2D& lineSeg, float thickness,
                                  const Rgba8* lineColorArray4, float duration )
    : DebugScreenObject( duration )
      , m_LineSegment( lineSeg )
      , m_Thickness( thickness )
{
    m_StartPointStartTint = lineColorArray4[ 0 ];
    m_StartPointEndTint = lineColorArray4[ 1 ];
    m_EndPointStartTint = lineColorArray4[ 2 ];
    m_EndPointEndTint = lineColorArray4[ 3 ];
}

void DebugScreenLine::RenderObject( RenderContext& ctx, const IntVec2& screenSize )
{
    UNUSED( screenSize );

    const Rgba8 startTint = Rgba8::LerpAsHSL( m_StartPointStartTint, m_StartPointEndTint,
                                         m_Duration->GetPercentage() );
    const Rgba8 endTint = Rgba8::LerpAsHSL( m_EndPointStartTint, m_StartPointEndTint,
                                       m_Duration->GetPercentage() );

    std::vector<VertexMaster> visual;
    AppendLine( visual, m_LineSegment, startTint, endTint, m_Thickness );

    ctx.DrawVertexArray( visual );
}

DebugScreenArrow::DebugScreenArrow( const LineSeg2D& lineSeg, const Rgba8* arrowColorArray4,
                                    const float duration )
    : DebugScreenObject( duration )
      , m_LineSegment( lineSeg )
{
    m_StartPointStartTint = arrowColorArray4[ 0 ];
    m_StartPointEndTint = arrowColorArray4[ 1 ];
    m_EndPointStartTint = arrowColorArray4[ 2 ];
    m_EndPointEndTint = arrowColorArray4[ 3 ];
}

void DebugScreenArrow::RenderObject( RenderContext& ctx, const IntVec2& screenSize )
{
    UNUSED( screenSize );

    const Rgba8 startTint = Rgba8::LerpAsHSL( m_StartPointStartTint, m_StartPointEndTint,
                                         m_Duration->GetPercentage() );
    const Rgba8 endTint = Rgba8::LerpAsHSL( m_EndPointStartTint, m_EndPointEndTint,
                                       m_Duration->GetPercentage() );

    const float thickness = m_LineSegment.GetLength() * ARROW_THICKNESS_TO_LENGTH;

    std::vector<VertexMaster> visual;
    AppendArrow( visual, m_LineSegment, startTint, endTint, thickness );

    ctx.DrawVertexArray( visual );
}

DebugScreenQuad::DebugScreenQuad( const AABB2& bounds, Texture* texture, const AABB2& uvs,
                                  const Rgba8& startTint, const Rgba8& endTint,
                                  const float duration )
    : DebugScreenObject( duration )
      , m_Bounds( bounds )
      , m_Uvs( uvs )
      , m_Texture( texture )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugScreenQuad::RenderObject( RenderContext& ctx, const IntVec2& screenSize )
{
    UNUSED( screenSize );

    const Vec2 centerPoint = m_Bounds.GetCenter();
    const Vec2 size = m_Bounds.GetDimensions();
    const Rgba8 tint = Rgba8::Lerp( m_StartTint, m_EndTint, m_Duration->GetPercentage() );

    std::vector<VertexMaster> visual;
    AppendPlaneSegment( visual, Vec3( centerPoint ), Vec3::I * size.x, Vec3::J * size.y, tint,
                        m_Uvs.mins, m_Uvs.maxs );

    ctx.BindTexture( m_Texture );
    ctx.DrawVertexArray( visual );
    ctx.BindTexture( nullptr );
}

DebugScreenTextObject::DebugScreenTextObject( const std::string& text, const Vec4& originAndOffset,
                                              const Vec2& pivot, float cellHeight, float cellAspect,
                                              const Rgba8& startTint, const Rgba8& endTint,
                                              float duration )
    : DebugScreenObject( duration )
      , m_Text( text )
      , m_OriginAndOffset( originAndOffset )
      , m_Pivot( pivot )
      , m_CellHeight( cellHeight )
      , m_CellAspect( cellAspect )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugScreenTextObject::RenderObject( RenderContext& ctx, const IntVec2& screenSize )
{
    const Vec2 screenSizef = static_cast<Vec2>(screenSize);
    const Vec2 textPosition = screenSizef * m_OriginAndOffset.XY() + m_OriginAndOffset.ZW();

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );
    float cellHeight = m_CellHeight;
    if ( cellHeight == CELL_HEIGHT_NOT_GIVEN )
    {
        cellHeight = screenSizef.y * DebugRenderer::m_ScreenFontHeightPercent;
    }

    std::vector<VertexMaster> visual;
    DebugRenderer::m_DebugFont->AddVertsForText( visual, m_Text, textPosition, m_Pivot, cellHeight,
                                                 tint, m_CellAspect );

    ctx.BindTexture( DebugRenderer::m_DebugFont->GetTexture() );
    ctx.DrawVertexArray( visual );
    ctx.BindTexture( nullptr );
}

DebugScreenBasis::DebugScreenBasis( const Vec4& originAndOffset, const Mat44& basis, const float scale,
                                    const Rgba8& startTint, const Rgba8& endTint, const float duration )
    : DebugScreenObject( duration )
      , m_OriginAndOffset( originAndOffset )
      , m_Basis( basis )
, m_Scale( scale )
      , m_StartTint( startTint )
      , m_EndTint( endTint )
{
}

void DebugScreenBasis::RenderObject( RenderContext& ctx, const IntVec2& screenSize )
{
    const Rgba8 lerpAmount = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );
    const Rgba8 iBasisColor = I_BASIS_COLOR * lerpAmount;
    const Rgba8 jBasisColor = J_BASIS_COLOR * lerpAmount;
    const Rgba8 kBasisColor = K_BASIS_COLOR * lerpAmount;

    const Vec3 position = Vec3(static_cast<Vec2>(screenSize) * m_OriginAndOffset.XY() + m_OriginAndOffset.ZW());
    const float thickness = ARROW_THICKNESS_TO_LENGTH * 4.f * m_Scale;

    std::vector<VertexMaster> visual;
    std::vector<unsigned int> index;
    AppendArrow3D( visual, index, LineSeg3D( position, position + m_Basis.GetIBasis3D() * m_Scale ),
                   iBasisColor, thickness );
    AppendArrow3D( visual, index, LineSeg3D( position, position + m_Basis.GetJBasis3D() * m_Scale ),
                   jBasisColor, thickness );
    AppendArrow3D( visual, index, LineSeg3D( position, position + m_Basis.GetKBasis3D() * m_Scale ),
                   kBasisColor, thickness );

    ctx.DrawIndexed( visual, index );
}

DebugWorldQuad::DebugWorldQuad( const Mat44& basis, const Vec2& ijSize, Texture* texture, const AABB2& uvs, const Rgba8& startTint, const Rgba8& endTint, float duration, RenderMode mode )
    : DebugWorldBasis( basis, startTint, endTint, duration, mode )
    , m_Size( ijSize )
    , m_Texture( texture )
    , m_Uvs( uvs )
{
}

void DebugWorldQuad::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    UNUSED( currentCamera );

    const Rgba8 tint = Rgba8::LerpAsHSL( m_StartTint, m_EndTint, m_Duration->GetPercentage() );

    std::vector<VertexMaster>visual;
    AppendPlaneSegment( visual, m_Basis, tint, m_Size, Vec2::ALIGN_CENTERED, m_Uvs.mins, m_Uvs.maxs );

    ctx.BindTexture( m_Texture );
    ctx.DrawVertexArray( visual );
    ctx.BindTexture( nullptr );
}

DebugWorldPlane::DebugWorldPlane( const Plane2D& plane, const Rgba8* planeColorArray4, float duration, RenderMode mode )
    : DebugWorldObject( duration, mode )
, m_Plane( plane )
{
    m_StartPlaneColor = planeColorArray4[ 0 ];
    m_EndPlaneColor = planeColorArray4[ 1 ];
    m_StartArrowColor = planeColorArray4[ 2 ];
    m_EndArrowColor = planeColorArray4[ 3 ];
}

void DebugWorldPlane::RenderObject( RenderContext& ctx, const Camera& currentCamera )
{
    const Mat44 cameraRotation = currentCamera.GetCameraModel();
    const Rgba8 planeTint = Rgba8::LerpAsHSL( m_StartPlaneColor, m_EndPlaneColor, m_Duration->GetPercentage() );
    const Rgba8 arrowTint = Rgba8::LerpAsHSL( m_StartArrowColor, m_EndArrowColor, m_Duration->GetPercentage() );

    const LineSeg3D arrow = LineSeg3D( cameraRotation.GetTranslation3D(),
                                       cameraRotation.GetTranslation3D() + Vec3( m_Plane.normal ) );
    std::vector<VertexMaster> visual;
    std::vector<unsigned int> indexes;
    AppendArrow3D( visual, indexes, arrow, arrowTint );
    ctx.DrawIndexed( visual, indexes );

    const Vec2 planeTangent = m_Plane.GetTangent();
    const Vec3 lineStart = currentCamera.GetWorldSpaceFromNdc( planeTangent * 2 );
    const Vec3 lineEnd = currentCamera.GetWorldSpaceFromNdc( planeTangent * -2 );

    visual.clear();
    AppendRay( visual, LineSeg3D( lineStart, lineEnd ), planeTint );
    ctx.DrawVertexArray( visual);

}
