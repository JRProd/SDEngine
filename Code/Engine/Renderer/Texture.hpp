#pragma once

#include "Engine/Core/Math/Primatives/IntVec2.hpp"

#include <vector>

//-----------------------------------------------------------------------------
// D3D11 Defines
struct ID3D11Texture2D;
// Engine Defines
class RenderContext;
class TextureView;
struct Rgba8;

enum class TextureViewType
{
    RENDER_TARGET_VIEW,
    SHADER_RESOURCE_VIEW,
    DEPTH_STENCIL_VIEW
};

struct TextureViewDescription
{
    TextureViewType type = TextureViewType::RENDER_TARGET_VIEW;

    bool operator==( const TextureViewDescription& rhs ) const;
};

class Texture
{
    friend class TextureView;

public:
    Texture( RenderContext* ctx, ID3D11Texture2D* handle );
    ~Texture();

    //-------------------------------------------------------------------------
    // Static Methods
    static Texture* CreateFromFile( RenderContext* context, const std::string& filePath, bool flipV = false );
    static Texture* CreateRenderTargetFromSize( RenderContext* context, const IntVec2& size );
    static Texture* CreateFromColor( RenderContext* context, const Rgba8& color );
    static Texture* CreateFromColorArray( RenderContext* context, const Rgba8* colorArray, const IntVec2& size );
    static Texture* CreateDepthBuffer( RenderContext* context, const IntVec2& size );

    TextureView* CreateOrGetView( const TextureViewDescription& description );
    TextureView* CreateOrGetRenderTargetView();
    TextureView* CreateOrGetShaderResourceView();
    TextureView* CreateOrGetDepthStencilView();

    //-----------------------------------------------------------------------------
    RenderContext* GetOwner() const { return m_Owner; }
    ID3D11Texture2D* GetHandle() const { return m_Handle; }
    float GetWidth() const { return static_cast<float>(m_TextureSize.x); }
    float GetHeight() const { return static_cast<float>(m_TextureSize.y); }

    const IntVec2 GetTextureSize() const { return m_TextureSize; }
    float GetAspectRatio() const;

private:
    RenderContext* m_Owner = nullptr;
    ID3D11Texture2D* m_Handle = nullptr;

    IntVec2 m_TextureSize = IntVec2::ZERO;

    std::vector<TextureView*> m_Views;
    TextureView* m_RenderTargetView = nullptr;
};