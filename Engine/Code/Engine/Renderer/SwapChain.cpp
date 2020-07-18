#include "SwapChain.hpp"

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/Texture.hpp"

//-----------------------------------------------------------------------------
SwapChain::SwapChain( RenderContext* owner, IDXGISwapChain* handle )
    : m_Owner(owner)
    , m_Handle(handle)
{
}

//-----------------------------------------------------------------------------
SwapChain::~SwapChain()
{
    if ( m_BackBuffer != nullptr )
    {
        delete m_BackBuffer;
        m_BackBuffer = nullptr;
    }

    m_Owner = nullptr;
    DX_SAFE_RELEASE( m_Handle );   
}

Texture* SwapChain::GetBackBuffer()
{
    if ( m_BackBuffer != nullptr )
    {
        return m_BackBuffer;
    }

    ID3D11Texture2D* texHandle = nullptr;
    m_Handle->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**) &texHandle );

    m_BackBuffer = new Texture( m_Owner, texHandle );

    return m_BackBuffer;
}

//-----------------------------------------------------------------------------
void SwapChain::Present( int vsync )
{
    m_Handle->Present( vsync, 0 );
}
