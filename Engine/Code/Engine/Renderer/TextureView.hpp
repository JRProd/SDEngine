#pragma once

#include "Texture.hpp"

//-----------------------------------------------------------------------------
// D3D11 Predefines
struct ID3D11Resource;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;

class TextureView
{
    friend class Texture;

public:
    ~TextureView();

    ID3D11RenderTargetView* GetAsRTV() const { return m_Rtv; }
    ID3D11ShaderResourceView* GetAsSRV() const { return m_Srv; }
    ID3D11DepthStencilView* GetAsDSV() const { return m_Dsv; }

private:
    union
    {
        ID3D11Resource* m_Handle;
        ID3D11RenderTargetView* m_Rtv;
        ID3D11ShaderResourceView* m_Srv;
        ID3D11DepthStencilView* m_Dsv;
    };

    TextureViewDescription m_Description;

    TextureView( Texture* texture, const TextureViewDescription& description );
};