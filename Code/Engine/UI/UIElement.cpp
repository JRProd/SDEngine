#include "UIElement.hpp"


#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"

AABB2 UISystem::g_DrawSpace;

UIElement::UIElement( UIElement* parent )
    : m_Parent( parent )
{
}

UIElement::UIElement( UIElement* parent, const AABB2& fixedBounds )
    : m_Parent( parent )
{
    if( parent != nullptr )
    {
        m_Parent->AddChild( this );
    }

    m_Positioning = Positioning::FIXED;

    m_ScreenBoundingBox = fixedBounds;
    m_DesiredSize = m_ScreenBoundingBox.GetDimensions();
}

UIElement::UIElement( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot )
    : m_OriginAndOffset( originAndOffset )
    , m_Pivot( pivot )
    , m_Parent( parent )
{
    if( parent != nullptr )
    {
        m_Parent->AddChild( this );
    }

    m_Positioning = Positioning::DYNAMIC;
}

UIElement::~UIElement()
{
    for( UIElement*& child : m_Children )
    {
        delete child;
        child = nullptr;
    }
}

void UIElement::PreUpdate()
{
    CalculateScreenPosition();

    for( UIElement* child : m_Children )
    {
        child->PreUpdate();
    }
}

UIElement* UIElement::GetRootElement()
{
    if( m_Parent == nullptr )
    {
        return this;
    }
    else
    {
        UIElement* parent = m_Parent;
        while( m_Parent->m_Parent != nullptr)
        {
            parent = m_Parent->m_Parent;
        }

        return parent;
    }

}

void UIElement::Update()
{
    if( m_Visibility == Visibility::HIDDEN ) { return; }

    if( m_Parent == nullptr )
    {
        PreUpdate();
    }

    Tick( this );

    for( UIElement* child : m_Children )
    {
        child->Update();
    }
}

void UIElement::Render( RenderContext& context )
{
    if( m_Visibility == Visibility::HIDDEN ) { return; }

    if( Draw.Empty() )
    {
        DefaultRender( context );
    }
    else
    {
        Draw( this );
    }

    for( UIElement* child : m_Children )
    {
        child->Render( context );
    }

    context.BindTexture( nullptr );
}

void UIElement::DebugRender( RenderContext& context )
{
    if( m_Visibility == Visibility::HIDDEN ) { return; }

    std::vector<VertexMaster> visual;
    AppendAABB2Perimeter( visual, m_ScreenBoundingBox, Rgba8::CYAN, 5.f );

    AppendCross( visual, m_ScreenBoundingBox.GetPointAtUV( m_Pivot ), 5.f, 2.5f, Rgba8::MAGENTA );

    context.BindTexture( nullptr );
    context.DrawVertexArray( visual );

    for( UIElement* child : m_Children )
    {
        child->DebugRender( context );
    }

    context.BindTexture( nullptr );
}

void UIElement::SetParent( UIElement* element )
{
    // Remove self from old parent
    if( m_Parent != nullptr )
    {
        m_Parent->RemoveChild( this );
    }
    m_Parent = element;

    // Add self to new parent
    if( m_Parent != nullptr )
    {
        m_Parent->AddChild( this );
    }
}

void UIElement::AddChild( UIElement* element )
{
    m_Children.push_back( element );
}

void UIElement::RemoveChild( UIElement* element )
{
    for( size_t childIndex = 0; childIndex < m_Children.size(); ++childIndex)
    {
        UIElement*& atIndex = m_Children.at( childIndex );
        if( atIndex == element )
        {
            atIndex = m_Children.back();
            m_Children.resize( m_Children.size() - 1 );
            break;
        }
    }
}

void UIElement::SetPivot( const Vec2& newPivot )
{
    m_Pivot = newPivot;
}

void UIElement::SetDesiredSize( const Vec2& newDesiredSize )
{
    m_DesiredSize = newDesiredSize;
}

void UIElement::SetOrientationAndOffset( const Vec4& newOriginAndOffset )
{
    m_Positioning = Positioning::DYNAMIC;
    m_OriginAndOffset = newOriginAndOffset;
}

void UIElement::SetFixedSize( const AABB2& newFixedSize )
{
    m_Positioning = Positioning::FIXED;
    m_ScreenBoundingBox = newFixedSize;
    m_DesiredSize = m_ScreenBoundingBox.GetDimensions();
}

void UIElement::SetVisibility( Visibility newVisibility )
{
    m_Visibility = newVisibility;
}

Visibility UIElement::GetVisibility() const
{
    return m_Visibility;
}

Vec2 UIElement::GetScreenPointFromNormalized(const Vec2& normalizedPoint) const
{
    return UISystem::g_DrawSpace.GetPointAtUV( normalizedPoint );
}

void UIElement::CalculateScreenPosition()
{
    if( m_Positioning == Positioning::FIXED ) { return; }

    AABB2 parentBox;
    // Top level UIElement
    if( m_Parent == nullptr )
    {
        // Take the offset from the bottom and the desired size
        parentBox = UISystem::g_DrawSpace;
    }
    else
    {
        parentBox = m_Parent->m_ScreenBoundingBox;
    }


    const Vec2 bottomLeft = parentBox.GetPointAtUV( m_OriginAndOffset.XY() ) + m_OriginAndOffset.ZW();
    const Vec2 topRight = bottomLeft + m_DesiredSize;
    const Vec2 offset = m_Pivot * m_DesiredSize;

    m_ScreenBoundingBox.mins = bottomLeft - offset;
    m_ScreenBoundingBox.maxs = topRight - offset;
}

void FontStyle::SetPadding( float all )
{
    padding = Vec4( all );
}

void FontStyle::SetPadding( float sides, float topBottom )
{
    padding = Vec4( sides, topBottom, sides, topBottom );
}

void FontStyle::SetPadding( float right, float top, float left, float bottom )
{
    padding = Vec4( right, top, left, bottom );
}
