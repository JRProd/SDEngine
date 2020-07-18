#pragma once
#include "Engine/UI/UIElement.hpp"

class UIFrame : public UIElement
{
public:
    UIFrame( UIElement* parent, const AABB2& fixedBounds );
    UIFrame( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot = Vec2::ALIGN_CENTERED );
    virtual ~UIFrame();

    void DefaultRender( RenderContext& context ) override;
};
