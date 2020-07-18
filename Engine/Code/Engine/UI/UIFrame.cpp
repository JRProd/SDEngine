#include "UIFrame.hpp"

#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"

UIFrame::UIFrame( UIElement* parent, const AABB2& fixedBounds )
    : UIElement( parent, fixedBounds )
{
}

UIFrame::UIFrame( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot )
    : UIElement( parent, originAndOffset, pivot )
{
}

UIFrame::~UIFrame()
{
}

void UIFrame::DefaultRender( RenderContext& context )
{
    UNUSED( context );
}
