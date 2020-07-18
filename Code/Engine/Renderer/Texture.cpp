#include "Texture.hpp"

#include "ThirdParty/stb/stb_image.h"

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Console/Console.hpp"

//-----------------------------------------------------------------------------
Texture::Texture( RenderContext* ctx, ID3D11Texture2D* handle )
    : m_Owner( ctx )
  , m_Handle( handle )
{
    D3D11_TEXTURE2D_DESC desc;
    handle->GetDesc( &desc );

    m_TextureSize = IntVec2( desc.Width, desc.Height );
}

//-----------------------------------------------------------------------------
Texture::~Texture()
{
    m_Owner = nullptr;
    DX_SAFE_RELEASE( m_Handle );

    for( TextureView*& view : m_Views )
    {
        if( view != nullptr )
        {
            delete view;
            view = nullptr;
        }
    }
}

STATIC Texture* Texture::CreateFromFile( RenderContext* context, const std::string& filePath, bool flipV )
{
    int imageTexelSizeX = 0;
    int imageTexelSizeY = 0;
    int numComponents = 0;
    const int numComponentsRequested = 4;

    stbi_set_flip_vertically_on_load( flipV );
    unsigned char* imageData = stbi_load( filePath.c_str(),
                                          &imageTexelSizeX,
                                          &imageTexelSizeY,
                                          &numComponents,
                                          numComponentsRequested
                                        );

    // Ensure that the data was correctly read in
    if( !imageData )
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "Failed too load image \"%s\"", filePath.c_str() ) );
        return nullptr;
#endif // !defined(ENGINE_DISABLE_CONSOLE)
#if defined(ENGINE_DISABLE_CONSOLE)
        ERROR_RECOVERABLE( Stringf( "Failed to load image \"%s\"", filePath.c_str() ) );
        return nullptr;
#endif // defined(ENGINE_DISABLE_CONSOLE)
    }

    if( !(numComponents == 3 || numComponents == 4) && imageTexelSizeX <= 0 && imageTexelSizeY <= 0 )
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR,
                        Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)",
                                 filePath.c_str(),
                                 numComponents,
                                 imageTexelSizeX,
                                 imageTexelSizeY
                               )
                      );
        return nullptr;
#endif // !defined(ENGINE_DISABLE_CONSOLE)
#if defined(ENGINE_DISABLE_CONSOLE)
        ERROR_RECOVERABLE( Stringf( "ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)",
                              filePath.c_str(),
                              numComponents,
                              imageTexelSizeX,
                              imageTexelSizeY )
                         );
        return nullptr;
#endif // defined(ENGINE_DISABLE_CONSOLE)
    }

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = imageTexelSizeX;
    desc.Height = imageTexelSizeY;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1; // MSAA
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE; // If using mip-chains, need to be GPU or dynamic
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = imageData;
    initialData.SysMemPitch = imageTexelSizeX * 4;
    initialData.SysMemSlicePitch = 0;

    ID3D11Device* device = context->m_Device;
    ID3D11Texture2D* texHandle = nullptr;
    device->CreateTexture2D( &desc, &initialData, &texHandle );

    stbi_image_free( imageData );

    return new Texture( context, texHandle );
}

Texture* Texture::CreateRenderTargetFromSize( RenderContext* context, const IntVec2& size )
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = size.x;
    desc.Height = size.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Device* device = context->m_Device;
    ID3D11Texture2D* texHandle = nullptr;
    device->CreateTexture2D( &desc, nullptr, &texHandle );

    return new Texture( context, texHandle );
}

Texture* Texture::CreateFromColor( RenderContext* context, const Rgba8& color )
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1; // MSAA
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE; // If using mip-chains, need to be GPU or dynamic
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = &color;
    initialData.SysMemPitch = 4;
    initialData.SysMemSlicePitch = 0;

    ID3D11Device* device = context->m_Device;
    ID3D11Texture2D* texHandle = nullptr;
    device->CreateTexture2D( &desc, &initialData, &texHandle );

    return new Texture( context, texHandle );
}

Texture* Texture::CreateFromColorArray( RenderContext* context, const Rgba8* colorArray, const IntVec2& size )
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = size.x;
    desc.Height = size.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1; // MSAA
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE; // If using mip-chains, need to be GPU or dynamic
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = colorArray;
    initialData.SysMemPitch = 4;
    initialData.SysMemSlicePitch = 0;

    ID3D11Device* device = context->m_Device;
    ID3D11Texture2D* texHandle = nullptr;
    device->CreateTexture2D( &desc, &initialData, &texHandle );

    return new Texture( context, texHandle );
}

Texture* Texture::CreateDepthBuffer( RenderContext* context, const IntVec2& size )
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = size.x;
    desc.Height = size.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D11Device* device = context->m_Device;
    ID3D11Texture2D* texHandle = nullptr;
    device->CreateTexture2D( &desc, nullptr, &texHandle );

    return new Texture( context, texHandle );
}

TextureView* Texture::CreateOrGetView( const TextureViewDescription& description )
{
    for( TextureView* view : m_Views )
    {
        if( view->m_Description == description )
        {
            return view;
        }
    }

    TextureView* view = new TextureView( this, description );
    m_Views.push_back( view );
    return view;
}

//-----------------------------------------------------------------------------
TextureView* Texture::CreateOrGetRenderTargetView()
{
    TextureViewDescription renderView;
    renderView.type = TextureViewType::RENDER_TARGET_VIEW;
    return CreateOrGetView( renderView );
}

TextureView* Texture::CreateOrGetShaderResourceView()
{
    TextureViewDescription renderView;
    renderView.type = TextureViewType::SHADER_RESOURCE_VIEW;
    return CreateOrGetView( renderView );
}

TextureView* Texture::CreateOrGetDepthStencilView()
{
    TextureViewDescription renderView;
    renderView.type = TextureViewType::DEPTH_STENCIL_VIEW;
    return CreateOrGetView( renderView );
}

//-----------------------------------------------------------------------------
float Texture::GetAspectRatio() const
{
    return static_cast<float>(m_TextureSize.x) /
    static_cast<float>(m_TextureSize.y);
}

bool TextureViewDescription::operator==( const TextureViewDescription& rhs ) const
{
    return type == rhs.type;
}
