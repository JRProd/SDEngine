#include "UIText.hpp"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"

UIText::UIText( UIElement* parent, const Vec4& originAndOffset )
    : UIElement( parent, originAndOffset )
{
    m_Text = "UIText";
}

UIText::UIText( UIElement* parent, const Vec4& originAndOffset, const char* text )
    : UIElement( parent, originAndOffset )
{
    m_Text = text;
}

UIText::UIText( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const char* text )
    : UIElement( parent, originAndOffset, pivot )
{
    m_Text = text;
}

UIText::UIText( UIElement* parent, const Vec4& originAndOffset, const std::string& text )
    : UIElement( parent, originAndOffset )
{
    m_Text = text;
}

UIText::UIText( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const std::string& text )
    : UIElement( parent, originAndOffset, pivot )
{
    m_Text = text;
}

void UIText::SetText( const std::string& newText )
{
    m_Text = newText;
}

void UIText::SetFontStyle( const FontStyle& fontStyle )
{
    m_FontStyle = fontStyle;
}

void UIText::SetFontSheet( BitmapFont* fontSheet )
{
    m_FontStyle.fontSheet = fontSheet;
}

void UIText::PreUpdate()
{
    const Vec2 fontDimens = m_FontStyle.fontSheet->GetDimensionForText( m_Text, m_FontStyle.fontHeight, m_FontStyle.fontAspect );
    const Vec4 padding = m_FontStyle.GetPadding();
    if( fontDimens.x > m_DesiredSize.x + padding.x + padding.z || fontDimens.y > m_DesiredSize.y + padding.y + padding.w )
    {
        m_DesiredSize = fontDimens + Vec2(padding.x + padding.z, padding.y + padding.w) ;
    }

    UIElement::PreUpdate();
}

void UIText::DefaultRender( RenderContext& context )
{
    std::vector<VertexMaster> visual;

    if( m_FontStyle.fontBackground.a != 0 )
    {
        std::vector<VertexMaster> background;
        AppendAABB2( background, m_ScreenBoundingBox, m_FontStyle.fontBackground );
        context.BindTexture( nullptr );
        context.DrawVertexArray( background );
    }

    const Rgba8 currentColor = m_FontStyle.fontTint;
    const Vec4 padding = m_FontStyle.GetPadding();
    AABB2 paddedBox = m_ScreenBoundingBox;
    paddedBox.mins += Vec2( padding.z, padding.w );
    paddedBox.maxs -= Vec2( padding.x, padding.y );

    m_FontStyle.fontSheet->AddVertsForMarkedTextInBox( visual, m_Text, paddedBox, m_FontStyle.fontHeight, m_FontStyle.pivot, currentColor, m_FontStyle.fontAspect );
    context.BindTexture( m_FontStyle.fontSheet->GetTexture() );
    context.DrawVertexArray( visual );
    context.BindTexture( nullptr );
}
