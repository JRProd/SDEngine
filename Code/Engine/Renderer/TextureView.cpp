#include "TextureView.hpp"

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

TextureView::~TextureView()
{
    DX_SAFE_RELEASE( m_Handle );
}

TextureView::TextureView( Texture* texture, const TextureViewDescription& description )
    : m_Description( description )
{
    ID3D11Device* device = texture->m_Owner->m_Device;

    switch ( m_Description.type )
    {
        case TextureViewType::RENDER_TARGET_VIEW:
            device->CreateRenderTargetView( texture->m_Handle, nullptr, &m_Rtv );
            break;
        case TextureViewType::SHADER_RESOURCE_VIEW:
            device->CreateShaderResourceView( texture->m_Handle, nullptr, &m_Srv );
            break;
        case TextureViewType::DEPTH_STENCIL_VIEW:
            device->CreateDepthStencilView( texture->m_Handle, nullptr, &m_Dsv );
            break;
    }
}