#include "UIImage.hpp"

#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Mesh/MeshUtils.hpp"

UIImage::UIImage( UIElement* parent, const AABB2& fixedSize, Texture* texture )
    : UIElement( parent, fixedSize )
  , m_Texture( texture )
{
}

UIImage::UIImage( UIElement* parent, const AABB2& fixedSize, const SpriteDefinition* sprite )
    : UIElement( parent, fixedSize )
  , m_Sprite( sprite )
{
}

UIImage::UIImage( UIElement* parent, const Vec4& originAndOffset, Texture* texture )
    : UIElement( parent, originAndOffset )
  , m_Texture( texture )
{
}

UIImage::UIImage( UIElement* parent, const Vec4& originAndOffset, const SpriteDefinition* sprite )
    : UIElement( parent, originAndOffset )
  , m_Sprite( sprite )
{
}

UIImage::UIImage( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, Texture* texture )
    : UIElement( parent, originAndOffset, pivot )
  , m_Texture( texture )
{
}

UIImage::UIImage( UIElement* parent, const Vec4& originAndOffset, const Vec2& pivot, const SpriteDefinition* sprite )
    : UIElement( parent, originAndOffset, pivot )
  , m_Sprite( sprite )
{
}


Vec2 UIImage::GetImageDimensions() const
{
    if( m_Texture != nullptr )
    {
        return static_cast<Vec2>(m_Texture->GetTextureSize());
    }

    if( m_Sprite != nullptr )
    {
        Vec2 uvAtMins, uvAtMaxs;
        m_Sprite->GetUVs( uvAtMins, uvAtMaxs );

        const Vec2& uvDimens = GetUVDimensions();
        const Vec2& imageSize = static_cast<Vec2>(m_Sprite->GetTexture().GetTextureSize());

        return imageSize * uvDimens;
    }

    ERROR_AND_DIE( "UIImage::GetImageDimensions - No image or sprite definition set" );
}

void UIImage::GetUVs( OUT_PARAM Vec2& uvAtMin, OUT_PARAM Vec2& uvAtMax ) const
{
    if( m_Texture != nullptr )
    {
        uvAtMin = Vec2::ZERO;
        uvAtMax = Vec2::ONE;
    }

    if( m_Sprite != nullptr )
    {
        m_Sprite->GetUVs( uvAtMin, uvAtMax );
        return;
    }
}

Vec2 UIImage::GetUVDimensions() const
{
    if( m_Texture != nullptr )
    {
        return Vec2::ONE;
    }

    if( m_Sprite != nullptr )
    {
        Vec2 uvAtMins, uvAtMaxs;
        m_Sprite->GetUVs( uvAtMins, uvAtMaxs );

        return Vec2( uvAtMaxs.x - uvAtMins.x, uvAtMins.y - uvAtMaxs.y );
    }

    ERROR_AND_DIE( "UIImage::GetUVDimensions - No image or sprite definition set" );
}

void UIImage::GetImageDisplayedBounds( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const
{
    switch( m_ScalingSetting )
    {
        case ImageScaling::STRETCH:
            plane = m_ScreenBoundingBox;
            GetUVs( uvs.mins, uvs.maxs );
            break;
        case ImageScaling::NATIVE:
            NativeCalculation( plane, uvs );
            break;
        case ImageScaling::FIT:
            GetUVs( uvs.mins, uvs.maxs );
            plane = AABB2( m_ScreenBoundingBox.mins,
                           m_ScreenBoundingBox.mins + GetImageDimensions()
                         );
            plane.ScaleToFitWithinBounds( m_ScreenBoundingBox, m_Pivot );
            break;
        case ImageScaling::FIT_HORIZONTAL: FitHorizontalCalculation( plane, uvs );
            break;
        case ImageScaling::FIT_VERTICAL: FitVerticalCalculation( plane, uvs );
            break;
    }
}

void UIImage::DefaultRender( RenderContext& context )
{
    AABB2 plane, uvs;
    GetImageDisplayedBounds( plane, uvs );

    std::vector<VertexMaster> visual;
    AppendAABB2( visual, plane, Rgba8::WHITE, uvs.mins, uvs.maxs );

    const Texture* textureToBind = m_Texture != nullptr ? m_Texture : &m_Sprite->GetTexture();
    context.BindTexture( textureToBind );
    context.DrawVertexArray( visual );
}

void UIImage::SetImageScaling( ImageScaling newScalingSetting )
{
    m_ScalingSetting = newScalingSetting;
}

void UIImage::NativeCalculation( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const
{
    const Vec2 imageSize = GetImageDimensions();

    Vec2 uvAtMin, uvAtMax;
    GetUVs( uvAtMin, uvAtMax );
    const Vec2& uvDimens = GetUVDimensions();
    // If the native resolution is greater than the desired size
    if( imageSize.x > m_DesiredSize.x )
    {
        plane.mins.x = m_ScreenBoundingBox.mins.x;
        plane.maxs.x = m_ScreenBoundingBox.maxs.x;

        const float uvRange = m_ScreenBoundingBox.GetDimensions().x / imageSize.x;
        const float valueLeft = uvDimens.x - uvRange;

        uvs.mins.x = valueLeft * m_Pivot.x;
        uvs.maxs.x = uvs.mins.x + uvRange;
    }
    else
    {
        uvs.mins.x = uvAtMin.x;
        uvs.maxs.x = uvAtMax.x;

        const float valueLeft = m_ScreenBoundingBox.GetDimensions().x - imageSize.x;

        plane.mins.x = m_ScreenBoundingBox.mins.x + valueLeft * m_Pivot.x;
        plane.maxs.x = plane.mins.x + imageSize.x;
    }

    if( imageSize.y > m_DesiredSize.y )
    {
        plane.mins.y = m_ScreenBoundingBox.mins.y;
        plane.maxs.y = m_ScreenBoundingBox.maxs.y;

        const float uvRange = m_ScreenBoundingBox.GetDimensions().y / imageSize.y;
        const float valueLeft = uvDimens.y - uvRange;

        uvs.mins.y = valueLeft * m_Pivot.y;
        uvs.maxs.y = uvs.mins.y + uvRange;
    }
    else
    {
        uvs.mins.y = uvAtMin.y;
        uvs.maxs.y = uvAtMax.y;

        const float valueLeft = m_ScreenBoundingBox.GetDimensions().y - imageSize.y;

        plane.mins.y = m_ScreenBoundingBox.mins.y + valueLeft * m_Pivot.y;
        plane.maxs.y = plane.mins.y + imageSize.y;
    }
}

void UIImage::FitHorizontalCalculation( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const
{
    Vec2 uvAtBottomLeft, uvAtTopRight;
    GetUVs( uvAtBottomLeft, uvAtTopRight );
    const Vec2& uvDimens = GetUVDimensions();

    // Uvs are guaranteed by the scale factor
    uvs.mins.x = uvAtBottomLeft.x;
    uvs.maxs.x = uvAtTopRight.x;


    Vec2 imageSize = GetImageDimensions();
    plane = AABB2( m_ScreenBoundingBox.mins, m_ScreenBoundingBox.mins + imageSize );

    // Determine the scale on the horizontal axis
    const float horizontalBounds = m_ScreenBoundingBox.GetDimensions().x;
    const float horizontalDifference = imageSize.x - horizontalBounds;

    const float scaleValue = (imageSize.x - horizontalDifference) / imageSize.x;
    plane.ScaleFromMin( scaleValue );

    // Calculate the vertical parameters based on the new dimension
    imageSize = plane.GetDimensions();
    if( imageSize.y > m_DesiredSize.y )
    {
        plane.mins.y = m_ScreenBoundingBox.mins.y;
        plane.maxs.y = m_ScreenBoundingBox.maxs.y;

        const float uvRange = m_ScreenBoundingBox.GetDimensions().y / imageSize.y;
        const float valueLeft = uvDimens.y - uvRange;

        uvs.mins.y = valueLeft * m_Pivot.y;
        uvs.maxs.y = uvs.mins.y + uvRange;
    }
    else
    {
        uvs.mins.y = uvAtBottomLeft.y;
        uvs.maxs.y = uvAtTopRight.y;

        const float valueLeft = m_ScreenBoundingBox.GetDimensions().y - imageSize.y;

        plane.mins.y = m_ScreenBoundingBox.mins.y + valueLeft * m_Pivot.y;
        plane.maxs.y = plane.mins.y + imageSize.y;
    }
}

void UIImage::FitVerticalCalculation( OUT_PARAM AABB2& plane, OUT_PARAM AABB2& uvs ) const
{
    Vec2 uvAtMin, uvAtMax;
    GetUVs( uvAtMin, uvAtMax );
    const Vec2& uvDimens = GetUVDimensions();

    // Uvs are guaranteed by the scale factor
    uvs.mins.y = uvAtMin.y;
    uvs.maxs.y = uvAtMax.y;

    Vec2 imageSize = GetImageDimensions();

    plane = AABB2( m_ScreenBoundingBox.mins, m_ScreenBoundingBox.mins + imageSize );

    // Determine the scale on the vertical axis
    const float verticalBounds = m_ScreenBoundingBox.GetDimensions().y;
    const float verticalDifference = imageSize.y - verticalBounds;

    const float scaleValue = (imageSize.y - verticalDifference) / imageSize.y;
    plane.ScaleFromMin( scaleValue );

    // Calculate the horizontal parameters based on the new dimension
    imageSize = plane.GetDimensions();
    if( imageSize.x > m_DesiredSize.x )
    {
        plane.mins.x = m_ScreenBoundingBox.mins.x;
        plane.maxs.x = m_ScreenBoundingBox.maxs.x;

        const float uvRange = m_ScreenBoundingBox.GetDimensions().x / imageSize.x;
        const float valueLeft = uvDimens.x - uvRange;

        uvs.mins.x = valueLeft * m_Pivot.x;
        uvs.maxs.x = uvs.mins.x + uvRange;
    }
    else
    {
        uvs.mins.x = uvAtMin.x;
        uvs.maxs.x = uvAtMax.x;

        const float valueLeft = m_ScreenBoundingBox.GetDimensions().x - imageSize.x;

        plane.mins.x = m_ScreenBoundingBox.mins.x + valueLeft * m_Pivot.x;
        plane.maxs.x = plane.mins.x + imageSize.x;
    }
}
