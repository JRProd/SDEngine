#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/UI/UIElement.hpp"

class Texture;

enum class ImageScaling
{
    STRETCH,
    FIT_HORIZONTAL,
    FIT_VERTICAL,
    FIT,
    NATIVE,
};

class UIImage : public UIElement
{
public:
    UIImage( UIElement* parent, const AABB2& fixedSize, Texture* texture );
    UIImage( UIElement* parent, const AABB2& fixedSize, const SpriteDefinition* sprite );
    UIImage( UIElement* parent, const Vec4& originAndOffset, Texture* texture );
    UIImage( UIElement* parent, const Vec4& originAndOffset, const SpriteDefinition* sprite );
    UIImage( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, Texture* texture );
    UIImage( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const SpriteDefinition* sprite );

    Vec2 GetImageDimensions() const;
    void GetUVs( OUT_PARAM Vec2& uvAtMin, OUT_PARAM Vec2& uvAtMax ) const;
    Vec2 GetUVDimensions() const;
    void GetImageDisplayedBounds( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs) const;
    void DefaultRender(RenderContext& context) override;

    void SetImageScaling( ImageScaling newScalingSetting );

private:
    Texture* m_Texture = nullptr;
    const SpriteDefinition* m_Sprite = nullptr;
    ImageScaling m_ScalingSetting = ImageScaling::FIT_VERTICAL;

    void NativeCalculation( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const;
    void FitHorizontalCalculation( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const;
    void FitVerticalCalculation( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const;
};
