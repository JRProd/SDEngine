#pragma once
#include <vector>
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/LineSeg2D.hpp"
#include "Engine/Core/Math/Primatives/LineSeg3D.hpp"
#include "Engine/Core/Math/Primatives/Mat44.hpp"
#include "Engine/Core/Math/Primatives/Plane2D.hpp"
#include "Engine/Core/Math/Primatives/Vec3.hpp"
#include "Engine/Core/Time/Timer.hpp"

#include "GPUMesh.hpp"
#include "Shaders/ShaderProgram.hpp"

struct Plane2D;
struct Transform;
class DebugScreenObject;
class BitmapFont;
struct Mat44;
struct LineSeg3D;
class Texture;
class Camera;

//TODO: Move to a more appropriate place
enum RenderMode
{
    RENDER_ALWAYS,
    RENDER_USE_DEPTH,
    RENDER_X_RAY,

    RENDER_NUM_MODES,
};

class DebugWorldObject;

class DebugRenderer
{
public:
    static Clock* m_DebugClock;
    static GPUMesh* m_Cube;
    static GPUMesh* m_Sphere;

    static BitmapFont* m_DebugFont;
    static float m_WorldFontHeight;
    static float m_WorldFontAspect;
    static float m_ScreenFontHeightPercent;
    static float m_ScreenFontAspect;

    static void Startup();
    static void Shutdown();

    static void Enable();
    static void Disable();
    static void Clear();

    static void BeginFrame();
    static void RenderToCamera( Camera& camera );
    static void RenderToTexture( Texture& texture );
    static void EndFrame();

    //-----------------------------------------------------------------------------
    static void AddWorldPoint( const Vec3& position,
                               float size,
                               const Rgba8& startTint,
                               const Rgba8& endTint,
                               float duration,
                               RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldPoint( const Vec3& position,
                               float size,
                               const Rgba8& tint,
                               float duration = 0.f,
                               RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldPoint( const Vec3& position,
                               const Rgba8& tint,
                               float duration = 0.f,
                               RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldRay( const LineSeg3D& line, const Rgba8& startPointStartTint,
                             const Rgba8& startPointEndTint,
                             const Rgba8& endPointStartTint, const Rgba8& endPointEndTint,
                             float thickness, float duration, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldRay( const LineSeg3D& line, const Rgba8& startPointTint,
                             const Rgba8& endPointTint, float thickness = 1.f, float duration = 0.f,
                             RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldRay( const LineSeg3D& line, const Rgba8& tint, float thickness = 1.f,
                             float duration = 0.f, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldRayTimeColorDecay( const LineSeg3D& line, const Rgba8& startTimeTint,
                                           const Rgba8& endTimeTint, float duration,
                                           float thickness = 1.f,
                                           RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldArrow( const LineSeg3D& line, const Rgba8& startPointStartTint,
                               const Rgba8& startPointEndTint, const Rgba8& endPointStartTint,
                               const Rgba8& endPointEndTint,
                               float duration, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldArrow( const LineSeg3D& line, const Rgba8& startPointTint,
                               const Rgba8& endPointTint, float duration = 0.f,
                               RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldArrow( const LineSeg3D& line, const Rgba8& tint, float duration = 0.f,
                               RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldArrowTimeColorDelay( const LineSeg3D& line, const Rgba8& startTimeTint,
                                             const Rgba8& endTimeTint, float duration,
                                             RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldPlane( const Plane2D& plane, const Rgba8& startPlaneTint,
                               const Rgba8& endPlaneTint, const Rgba8& startArrowTint,
                               const Rgba8& endArrowTint, float duration,
                               RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldPlane( const Plane2D& plane, const Rgba8& planeTint, const Rgba8& arrowTint,
                               float duration = 0.f, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldPlane( const Plane2D& plane, const Rgba8& tint, float duration = 0.f,
                               RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldPlane( const Plane2D& plane, float duration = 0.f,
                               RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldQuad( const Mat44& basis, const Vec2& ijSize, Texture* texture,
                              const AABB2& uvs, const Rgba8& startTint, const Rgba8& endTint,
                              float duration, RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldCubeBounds( const Transform& transform, const Rgba8& startTint,
                                    const Rgba8& endTint, float duration,
                                    RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldCubeBounds( const Transform& transform, const Rgba8& tint = Rgba8::YELLOW,
                                    float duration = 0.f, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldCubeBounds( const Vec3& position, const Vec3& rotation, const Vec3& scale,
                                    const Rgba8& startTint, const Rgba8& endTint, float duration,
                                    RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldCubeBounds( const Vec3& position, const Vec3& rotation = Vec3::ZERO,
                                    const Vec3& scale = Vec3::ONE,
                                    const Rgba8& tint = Rgba8::YELLOW, float duration = 0.f,
                                    RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldCubeBounds( const Vec3& position, const Vec3& rotation,
                                    const Rgba8& startTint, const Rgba8& endTint, float duration,
                                    RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldCubeBounds( const Vec3& position, const Rgba8& startTint,
                                    const Rgba8& endTint, float duration,
                                    RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldSphereBounds( const Transform& transform, const Rgba8& startTint,
                                      const Rgba8& endTint, float duration,
                                      RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldSphereBounds( const Transform& transform, const Rgba8& tint = Rgba8::YELLOW,
                                      float duration = 0.f, RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldSphereBounds( const Vec3& position, const Vec3& rotation, const Vec3& scale,
                                      const Rgba8& startTint, const Rgba8& endTint, float duration,
                                      RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldSphereBounds( const Vec3& position, const Vec3& rotation, float radius,
                                      const Rgba8& startTint, const Rgba8& endTint, float duration,
                                      RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldSphereBounds( const Vec3& position, const Vec3& rotation = Vec3::ZERO,
                                      const Vec3& scale = Vec3::ONE,
                                      const Rgba8& tint = Rgba8::YELLOW, float duration = 0.f,
                                      RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldSphereBounds( const Vec3& position, const Vec3& rotation = Vec3::ZERO,
                                      float radius = 1.f, const Rgba8& tint = Rgba8::YELLOW,
                                      float duration = 0.f, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldSphereBounds( const Vec3& position, const Vec3& rotation,
                                      const Rgba8& startTint, const Rgba8& endTint, float duration,
                                      RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldSphereBounds( const Vec3& position, const Rgba8& startTint,
                                      const Rgba8& endTint, float duration,
                                      RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldBasis( const Mat44& basis, const Rgba8& startTint, const Rgba8& endTint,
                               float duration, RenderMode mode = RENDER_USE_DEPTH );
    static void AddWorldBasis( const Mat44& basis, float duration,
                               RenderMode mode = RENDER_USE_DEPTH );

    static void AddWorldText( const Mat44& basis, const Vec2& pivot, const Rgba8& startTint,
                              const Rgba8& endTint, float duration, RenderMode mode,
                              const char* text );
    static void AddWorldText( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                              float duration, const char* text );
    static void AddWorldText( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                              const char* text );
    static void AddWorldText( const Mat44& basis, const char* text );
    static void AddWorldTextf( const Mat44& basis, const Vec2& pivot, const Rgba8& startTint,
                               const Rgba8& endTint, float duration, const char* format, ... );
    static void AddWorldTextf( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                               float duration, const char* format, ... );
    static void AddWorldTextf( const Mat44& basis, const Vec2& pivot, const Rgba8& tint,
                               const char* format, ... );
    static void AddWorldTextf( const Mat44& basis, const char* format, ... );

    static void AddBillboardText( const Vec3& origin, const Vec2& pivot, const Rgba8& startTint,
                                  const Rgba8& endTint, float duration, RenderMode mode,
                                  const char* text );
    static void AddBillboardText( const Vec3& origin, const Vec2& pivot, const Rgba8& tint,
                                  float duration, const char* text );
    static void AddBillboardText( const Vec3& origin, const Vec2& pivot, const Rgba8& tint,
                                  const char* text );
    static void AddBillboardText( const Vec3& origin, const char* text );
    static void AddBillboardTextf( const Vec3& origin, const Vec2& pivot, const Rgba8& startTint,
                                   const Rgba8& endTint, float duration, const char* format, ... );
    static void AddBillboardTextf( const Vec3& origin, const Vec2& pivot, const Rgba8& tint,
                                   float duration, const char* format, ... );
    static void AddBillboardTextf( const Vec3& origin, const Vec2& pivot, const Rgba8& tint,
                                   const char* format, ... );
    static void AddBillboardTextf( const Vec3& origin, const char* format, ... );

    //----------

    static void AddScreenPoint( const Vec2& position, float size, const Rgba8& startTint,
                                const Rgba8& endTint, float duration );
    static void AddScreenPoint( const Vec2& position, float size,
                                const Rgba8& tint = Rgba8::WHITE, float duration = 0.f );
    static void AddScreenPoint( const Vec2& position, const Rgba8& tint = Rgba8::WHITE,
                                float duration = 0.f );

    static void AddScreenLine( const LineSeg2D& lineSeg, const Rgba8& startPointStartTint,
                               const Rgba8& startPointEndTint, const Rgba8& endPointStartTint,
                               const Rgba8& endPointEndTint, float thickness, float duration );
    static void AddScreenLine( const LineSeg2D& lineSeg, const Rgba8& startPointTint,
                               const Rgba8& endPointTint, float thickness = 1.f,
                               float duration = 0.f );
    static void AddScreenLine( const LineSeg2D& lineSeg, const Rgba8& tint,
                               float thickness = 1.f, float duration = 0.f );
    static void AddScreenLine( const LineSeg2D& lineSeg, float thickness = 1.f,
                               const Rgba8& tint = Rgba8::WHITE, float duration = 0.f );
    static void AddScreenLineTimeColorDelay( const LineSeg2D& lineSeg,
                                             const Rgba8& startTimeTint,
                                             const Rgba8& endTimeTint, float thickness = 1.f,
                                             float duration = 0.f );

    static void AddScreenArrow( const LineSeg2D& lineSeg, const Rgba8& startPointStartTint,
                                const Rgba8& startPointEndTint, const Rgba8& endPointStartTint,
                                const Rgba8& endPointEndTint, float duration );
    static void AddScreenArrow( const LineSeg2D& lineSeg, const Rgba8& startPointTint,
                                const Rgba8& endPointTint, float duration = 0.f );
    static void AddScreenArrow( const LineSeg2D& lineSeg, const Rgba8& tint = Rgba8::WHITE,
                                float duration = 0.f );
    static void AddScreenArrowTimeColorDelay( const LineSeg2D& lineSeg,
                                              const Rgba8& startTimeTint,
                                              const Rgba8& endTimeTint, float duration = 0.f );

    static void AddScreenQuad( const AABB2& bounds, Texture* texture, const AABB2& uvs,
                               const Rgba8& startTint, const Rgba8& endTint, float duration );
    static void AddScreenQuad( const AABB2& bounds, Texture* texture, const AABB2& uvs,
                               const Rgba8& tint = Rgba8::WHITE, float duration = 0.f );
    static void AddScreenQuad( const AABB2& bounds, Texture* texture, const Rgba8& startTint,
                               const Rgba8& endTint, float duration );
    static void AddScreenQuad( const AABB2& bounds, Texture* texture,
                               const Rgba8& tint = Rgba8::WHITE, float duration = 0.f );
    static void AddScreenQuad( const AABB2& bounds, const Rgba8& startTint,
                               const Rgba8& endTint, float duration );
    static void AddScreenQuad( const AABB2& bounds, const Rgba8& tint = Rgba8::WHITE,
                               float duration = 0.f );

    static void AddScreenText( const Vec4& originAndOffset, const Vec2& pivot, float cellHeight,
                               float cellAspect, const Rgba8& startTint, const Rgba8& endTint,
                               float duration, const char* text );
    static void AddScreenText( const Vec4& originAndOffset, const Vec2& pivot, float cellHeight,
                               float cellAspect, const Rgba8& tint, float duration,
                               const char* text );
    static void AddScreenText( const Vec4& originAndOffset, const Vec2& pivot, const Rgba8& tint,
                               float duration, const char* text );
    static void AddScreenText( const Vec4& originAndOffset, const char* text );
    static void AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot, float cellHeight,
                                float cellAspect, const Rgba8& startTint, const Rgba8& endTint,
                                float duration, const char* format, ... );
    static void AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot, float cellHeight,
                                float cellAspect, const Rgba8& tint, float duration,
                                const char* format, ... );
    static void AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot, const Rgba8& tint,
                                float duration, const char* format, ... );
    static void AddScreenTextf( const Vec4& originAndOffset, const Vec2& pivot, 
                                const char* format, ... );
    static void AddScreenTextf( const Vec4& originAndOffset, const char* format, ... );

    static void AddScreenBasis( const Vec4& originAndOffset, const Mat44& basis,
                                const Rgba8& startTint, const Rgba8& endTint, float duration );
    static void AddScreenBasis( const Vec4& originAndOffset, const Rgba8& startTint,
                                const Rgba8& endTint, float duration );
    static void AddScreenBasis( const Vec4& originAndOffset, const Mat44& basis = Mat44::IDENTITY,
                                const Rgba8& tint = Rgba8::WHITE, float duration = 0.f );

private:
    static std::vector<DebugWorldObject*> m_DebugObjects[ RENDER_NUM_MODES ];
    static std::vector<DebugScreenObject*> m_DebugScreenObjects;
    static Camera* m_CurrentCamera;

    static Shader* m_Depth;
    static Shader* m_XRay;

    static bool m_IsFinalized;
    static bool m_IsEnabled;

    static void AddWorldMesh( GPUMesh*& mesh, const Transform& transform, const Rgba8& startTint,
                              const Rgba8& endTint, float duration, RenderMode mode );

    static void RenderAlwaysObjects();
    static void RenderDepthObjects();
    static void RenderXRayObjects();

    static void RenderScreenObjects();

    static void Finalize();
};

class DebugWorldObject
{
public:
    DebugWorldObject( float duration, RenderMode mode );
    virtual ~DebugWorldObject();

    bool HasElapsed() const;

    virtual void RenderObject( RenderContext& ctx, const Camera& currentCamera ) =
    0;

protected:
    Timer* m_Duration = nullptr;

    RenderMode m_Mode = RENDER_USE_DEPTH;
};

class DebugWorldPoint: public DebugWorldObject
{
public:
    DebugWorldPoint( const Vec3& position, float size, const Rgba8& startTint, const Rgba8& endTint,
                     float duration, RenderMode mode );
    virtual ~DebugWorldPoint() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    Vec3 m_Position;
    float m_Size = 0.f;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};

class DebugWorldRay: public DebugWorldObject
{
public:
    DebugWorldRay( const LineSeg3D& lineSeg, float thickness, const Rgba8* lineColorArray4,
                   float duration,
                   RenderMode mode );
    virtual ~DebugWorldRay() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    LineSeg3D m_LineSegment;
    float m_Thickness;

    Rgba8 m_StartPointStartColor;
    Rgba8 m_StartPointEndColor;
    Rgba8 m_EndPointStartColor;
    Rgba8 m_EndPointEndColor;
};

class DebugWorldArrow: public DebugWorldObject
{
public:
    DebugWorldArrow( const LineSeg3D& lineSeg, const Rgba8* arrowColorArray4, float duration,
                     RenderMode mode );
    virtual ~DebugWorldArrow() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    LineSeg3D m_LineSegment;

    Rgba8 m_StartPointStartColor;
    Rgba8 m_StartPointEndColor;
    Rgba8 m_EndPointStartColor;
    Rgba8 m_EndPointEndColor;
};

class DebugWorldPlane: public DebugWorldObject
{
public:
    DebugWorldPlane( const Plane2D& plane, const Rgba8* planeColorArray4, float duration,
                     RenderMode mode );
    virtual ~DebugWorldPlane() = default;

    void RenderObject(RenderContext& ctx, const Camera& currentCamera ) override;

private:
    Plane2D m_Plane;

    Rgba8 m_StartPlaneColor;
    Rgba8 m_EndPlaneColor;
    Rgba8 m_StartArrowColor;
    Rgba8 m_EndArrowColor;
};

class DebugWorldBasis: public DebugWorldObject
{
public:
    DebugWorldBasis( const Mat44& basis, const Rgba8& startTint, const Rgba8& endTint,
                     float duration, RenderMode mode );
    virtual ~DebugWorldBasis() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

protected:
    Mat44 m_Basis;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};

class DebugWorldQuad: public DebugWorldBasis
{
public:
    DebugWorldQuad( const Mat44& basis, const Vec2& ijSize, Texture* texture, const AABB2& uvs,
                    const Rgba8& startTint, const Rgba8& endTint, float duration, RenderMode mode );
    virtual ~DebugWorldQuad() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    Vec2 m_Size;
    Texture* m_Texture = nullptr;
    AABB2 m_Uvs;
};

class DebugWorldMesh: public DebugWorldBasis
{
public:
    DebugWorldMesh( GPUMesh*& mesh, const Mat44& transform, const Rgba8& startTint,
                    const Rgba8& endTint, float duration, RenderMode mode );
    virtual ~DebugWorldMesh() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    GPUMesh*& m_Mesh;

    static Shader* m_WireframeShader;
    static bool m_Finalized;
};

class DebugWorldText: public DebugWorldBasis
{
public:
    DebugWorldText( const std::string& text, const Vec2& pivot, float cellHeight, float cellAspect,
                    const Mat44& basis, const Rgba8& startTint, const Rgba8& endTint,
                    float duration, RenderMode mode );
    virtual ~DebugWorldText() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    std::string m_Text;
    Vec2 m_Pivot = Vec2::ZERO;

    float m_CellHeight = DebugRenderer::m_WorldFontHeight;
    float m_CellAspect = DebugRenderer::m_WorldFontAspect;
};

class DebugBillboardText: public DebugWorldObject
{
public:
    DebugBillboardText( const std::string& text, const Vec3& origin, const Vec2& pivot,
                        float cellHeight, float cellAspect, const Rgba8& startTint,
                        const Rgba8& endTint, float duration, RenderMode mode );
    ~DebugBillboardText() = default;

    void RenderObject( RenderContext& ctx, const Camera& currentCamera ) override;

private:
    std::string m_Text;
    Vec3 m_Origin = Vec3::ZERO;
    Vec2 m_Pivot = Vec2::ZERO;

    float m_CellHeight = DebugRenderer::m_WorldFontHeight;
    float m_CellAspect = DebugRenderer::m_WorldFontAspect;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};


class DebugScreenObject
{
public:
    explicit DebugScreenObject( float duration );
    virtual ~DebugScreenObject() = default;

    bool HasElapsed() const;

    virtual void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) = 0;

protected:
    Timer* m_Duration = nullptr;
};

class DebugScreenPoint: public DebugScreenObject
{
public:
    DebugScreenPoint( const Vec2& position, float size, const Rgba8& startTint,
                      const Rgba8& endTint, float duration );
    virtual ~DebugScreenPoint() = default;

    void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) override;

private:
    Vec2 m_Position;
    float m_Size = 0.f;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};

class DebugScreenLine: public DebugScreenObject
{
public:
    DebugScreenLine( const LineSeg2D& lineSeg, float thickness, const Rgba8* lineColorArray4,
                     float duration );
    virtual ~DebugScreenLine() = default;

    void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) override;

private:
    LineSeg2D m_LineSegment;
    float m_Thickness = 0.f;

    Rgba8 m_StartPointStartTint;
    Rgba8 m_StartPointEndTint;
    Rgba8 m_EndPointStartTint;
    Rgba8 m_EndPointEndTint;
};

class DebugScreenArrow: public DebugScreenObject
{
public:
    DebugScreenArrow( const LineSeg2D& lineSeg, const Rgba8* arrowColorArray4, float duration );
    virtual ~DebugScreenArrow() = default;

    void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) override;

private:
    LineSeg2D m_LineSegment;

    Rgba8 m_StartPointStartTint;
    Rgba8 m_StartPointEndTint;
    Rgba8 m_EndPointStartTint;
    Rgba8 m_EndPointEndTint;
};

class DebugScreenQuad: public DebugScreenObject
{
public:
    DebugScreenQuad( const AABB2& bounds, Texture* texture, const AABB2& uvs,
                     const Rgba8& startTint, const Rgba8& endTint, float duration );
    virtual ~DebugScreenQuad() = default;

    void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) override;

private:
    AABB2 m_Bounds;
    AABB2 m_Uvs;
    Texture* m_Texture = nullptr;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};

class DebugScreenTextObject: public DebugScreenObject
{
public:
    DebugScreenTextObject( const std::string& text, const Vec4& originAndOffset, const Vec2& pivot,
                           float cellHeight, float cellAspect, const Rgba8& startTint,
                           const Rgba8& endTint, float duration );
    virtual ~DebugScreenTextObject() = default;

    void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) override;

private:
    std::string m_Text;
    Vec4 m_OriginAndOffset;
    Vec2 m_Pivot;

    float m_CellHeight = 0.f;
    float m_CellAspect = DebugRenderer::m_ScreenFontAspect;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};

class DebugScreenBasis: public DebugScreenObject
{
public:
    DebugScreenBasis( const Vec4& originAndOffset, const Mat44& basis, float scale,
                      const Rgba8& startTint, const Rgba8& endTint, float duration );
    virtual ~DebugScreenBasis() = default;

    void RenderObject( RenderContext& ctx, const IntVec2& screenSize ) override;

private:
    Vec4 m_OriginAndOffset;
    Mat44 m_Basis;
    float m_Scale = 1.f;

    Rgba8 m_StartTint;
    Rgba8 m_EndTint;
};
