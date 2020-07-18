#pragma once
#include "Engine/Core/Math/Primatives/AABB2.hpp"
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/Core/Math/Primatives/Vec4.hpp"
#include "Engine/Event/Delegate.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"



#include <vector>

class RenderContext;

enum class Visibility
{
    VISIBLE,
    HIDDEN,
};

enum class Positioning
{
    FIXED,
    DYNAMIC,
};

namespace UISystem
{
    extern AABB2 g_DrawSpace;
}

struct FontStyle
{
    float fontHeight = 18.f;
    float fontAspect = .56f;

    Rgba8 fontTint = Rgba8::WHITE;
    Rgba8 fontBackground = Rgba8::CLEAR;
    Vec2 pivot = Vec2::ALIGN_CENTERED;

    BitmapFont* fontSheet = nullptr;

    Vec4 GetPadding() const { return padding; }
    void SetPadding( float all );
    void SetPadding( float sides, float topBottom );
    void SetPadding( float right, float top, float left, float bottom );

private:
    Vec4 padding = Vec4::ZERO;
};

class UIElement
{
public:
    Delegate<UIElement*> Tick;
    Delegate<UIElement*> Draw;

    virtual ~UIElement();

    virtual void Update();
    void Render( RenderContext& context );
    void DebugRender( RenderContext& context );

    void SetParent( UIElement* element );
    void AddChild( UIElement* element );
    void RemoveChild( UIElement* element );

    void SetPivot( const Vec2& newPivot );
    Vec2 GetPivot() const { return m_Pivot; }

    void SetDesiredSize( const Vec2& newDesiredSize );
    Vec2 GetDesiredSize() const { return m_DesiredSize; }

    void SetOrientationAndOffset( const Vec4& newOriginAndOffset );
    Vec4 GetOriginAndOffset() const { return m_OriginAndOffset; }

    void SetFixedSize( const AABB2& newFixedSize);
    AABB2 GetFixedSize() const { return m_ScreenBoundingBox; }

    void SetVisibility( Visibility newVisibility );
    Visibility GetVisibility() const;

    AABB2 GetScreenBounds() const { return m_ScreenBoundingBox; }

    Vec2 GetScreenPointFromNormalized( const Vec2& normalizedPoint ) const;

protected:
    Visibility m_Visibility = Visibility::VISIBLE;
    Positioning m_Positioning = Positioning::DYNAMIC;

    Vec4 m_OriginAndOffset;
    Vec2 m_Pivot;
    Vec2 m_DesiredSize;
    AABB2 m_ScreenBoundingBox;

    UIElement( UIElement* parent );
    UIElement( UIElement* parent, const AABB2& fixedBounds );
    UIElement( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot = Vec2::ALIGN_CENTERED );

    virtual void PreUpdate();
    virtual void DefaultRender( RenderContext& context ) = 0;

    UIElement* GetRootElement();

private:
    UIElement* m_Parent = nullptr;
    std::vector<UIElement*> m_Children;

    void CalculateScreenPosition();
};
