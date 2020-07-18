#pragma once

//-----------------------------------------------------------------------------
// D3D11 Defines
struct IDXGISwapChain;
// Engine Defines
class RenderContext;
class Texture;

class SwapChain
{
public:
    SwapChain( RenderContext* owner, IDXGISwapChain* handle);
    ~SwapChain();

    //-----------------------------------------------------------------------------
    Texture* GetBackBuffer();

    //-----------------------------------------------------------------------------
    void Present( int vsync = 0 );


private:
    RenderContext* m_Owner;
    IDXGISwapChain* m_Handle;

    Texture* m_BackBuffer = nullptr;
};