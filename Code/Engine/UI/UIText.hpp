#pragma once
#include "Engine/UI/UIElement.hpp"

class UIText: public UIElement
{
public:
    UIText( UIElement* parent, const Vec4& originAndOffset );
    UIText( UIElement* parent, const Vec4& originAndOffset, const char* text );
    UIText( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const char* text );
    UIText( UIElement* parent, const Vec4& originAndOffset, const std::string& text );
    UIText( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const std::string& text );

    void SetText( const std::string& newText );
    std::string GetText() const { return m_Text; }

    void SetFontStyle( const FontStyle& fontStyle );
    FontStyle GetFontStyle() const { return m_FontStyle; }

    void SetFontSheet( BitmapFont* fontSheet );

    void PreUpdate() override;
    void DefaultRender(RenderContext& context) override;

private:
    std::string m_Text;

    FontStyle m_FontStyle;
};
