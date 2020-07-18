#pragma once
#include "Engine/Core/Math/Primatives/Vec2.hpp"
#include "Engine/UI/UIElement.hpp"

struct ButtonFontStyle: FontStyle
{
    Rgba8 fontHoveredTint = Rgba8::YELLOW;
    Rgba8 fontClickedTint = Rgba8::DARK_YELLOW;
};

class UIButton : public UIElement
{
public:
    Delegate<> OnHoverStart;
    Delegate<> OnHover;
    Delegate<> OnHoverEnd;
    Delegate<> OnClickStart;
    Delegate<> OnClick;
    Delegate<> OnClickEnd;

    UIButton( UIElement* parent, const Vec4& originAndOffset );
    UIButton( UIElement* parent, const Vec4& originAndOffset, const char* text );
    UIButton( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const char* text );
    UIButton( UIElement* parent, const Vec4& originAndOffset, const std::string& text );
    UIButton( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const std::string& text );

    void SetText( const std::string& newText );
    std::string GetText() const { return m_Text; }

    void SetFontStyle( const ButtonFontStyle& fontStyle );
    ButtonFontStyle GetFontStyle() const { return m_FontStyle; }

    void SetFontSheet( BitmapFont* fontSheet );

    void PreUpdate() override;
    void Update() override;

protected:
    void DefaultRender( RenderContext& context ) override;

private:
    std::string m_Text;
    ButtonFontStyle m_FontStyle;

    bool m_IsHovered = false;
    bool m_WasHoveredLastFrame = false;
    bool m_IsClicked = false;
    bool m_WasClickedLastFrame = false;
};
