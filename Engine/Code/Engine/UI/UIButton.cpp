#include "UIButton.hpp"


#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexTypes/VertexMaster.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"


UIButton::UIButton( UIElement* parent, const Vec4& originAndOffset )
    : UIElement( parent, originAndOffset )
{
    m_Text = "UIButton";
}

UIButton::UIButton( UIElement* parent, const Vec4& originAndOffset, const char* text )
    : UIButton( parent, originAndOffset )
{
    m_Text = text;
}

UIButton::UIButton( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const char* text )
    : UIElement( parent, originAndOffset, pivot )
{
    m_Text = text;
}

UIButton::UIButton( UIElement* parent, const Vec4& originAndOffset, const std::string& text )
    : UIButton( parent, originAndOffset )
{
    m_Text = text;
}

UIButton::UIButton( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const std::string& text )
    : UIElement( parent, originAndOffset, pivot )
{
    m_Text = text;
}

void UIButton::SetText( const std::string& newText )
{
    m_Text = newText;
}

void UIButton::SetFontStyle( const ButtonFontStyle& fontStyle )
{
    m_FontStyle = fontStyle;
}

void UIButton::SetFontSheet( BitmapFont* fontSheet )
{
    m_FontStyle.fontSheet = fontSheet;
}

void UIButton::PreUpdate()
{
    const Vec2 fontDimens = m_FontStyle.fontSheet->GetDimensionForText( m_Text, m_FontStyle.fontHeight, m_FontStyle.fontAspect );
    if( fontDimens.x > m_DesiredSize.x || fontDimens.y > m_DesiredSize.y )
    {
        m_DesiredSize = fontDimens;
    }

    UIElement::PreUpdate();
}

void UIButton::Update()
{
    const Vec2 normalizedMousePos = InputSystem::INSTANCE().GetMousePositionNormalized();
    const Vec2 mouseScreenLocation = UISystem::g_DrawSpace.GetPointAtUV( normalizedMousePos );

    m_IsHovered = m_ScreenBoundingBox.IsPointInside( mouseScreenLocation );
    m_IsClicked = InputSystem::INSTANCE().IsKeyPressed( MOUSE_L );

    if( m_IsHovered )
    {
        // If it wasnt hovered last frame
        if( !m_WasHoveredLastFrame )
        {
            OnHoverStart();
        }
        else
        {
            OnHover();
        }

        
        if( m_IsClicked )
        {
            if( !m_WasClickedLastFrame )
            {
                OnClickStart();
            }
            else
            {
                OnClick();
            }
        }
        else
        {
            if( m_WasClickedLastFrame )
            {
                OnClickEnd();
            }
        }
    }
    else
    {
        if( m_WasHoveredLastFrame && !m_IsHovered )
        {
            OnHoverEnd();
        }

        if( m_WasClickedLastFrame && !m_IsClicked )
        {
            OnClickEnd();
        }
    }

    m_WasHoveredLastFrame = m_IsHovered;
    m_WasClickedLastFrame = m_IsClicked;
}

void UIButton::DefaultRender( RenderContext& context )
{
    std::vector<VertexMaster> visual;

    AppendAABB2( visual, m_ScreenBoundingBox, Rgba8::GRAY );
    context.BindTexture( nullptr );
    context.DrawVertexArray( visual );

    visual.clear();

    Rgba8 currentColor = m_FontStyle.fontTint;
    if( m_IsHovered )
    {
        currentColor = m_FontStyle.fontHoveredTint;
    }
    if( m_IsClicked )
    {
        currentColor = m_FontStyle.fontClickedTint;
    }
    m_FontStyle.fontSheet->AddVertsForMarkedTextInBox( visual, m_Text, m_ScreenBoundingBox, m_FontStyle.fontHeight, Vec2::ALIGN_CENTERED, currentColor, m_FontStyle.fontAspect );
    context.BindTexture( m_FontStyle.fontSheet->GetTexture() );
    context.DrawVertexArray( visual );
}
