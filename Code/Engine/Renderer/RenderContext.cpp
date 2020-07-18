#include "RenderContext.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

#include "D3D11Common.hpp"

//-----------------------------------------------------------------------------
// Internal includes
#include "Engine/Console/Console.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Math/Primatives/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"
#include "Engine/Core/VertexTypes/Vertex_PCUTBN.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/OS/Window.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"
#include "Engine/Renderer/Sprite/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SwapChain.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

#include "Buffers/ConstantBuffer.hpp"
#include "Buffers/IndexBuffer.hpp"
#include "Material.hpp"
#include "Light/Light.hpp"

struct ModelDataUBO;

//-------------------------------------------------------------------------------
void RenderContext::Startup( Window* window )
{
#if defined(RENDER_DEBUG)
    CreateDebugModule();
#endif


    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if defined(RENDER_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    IDXGISwapChain* swapChain;
    DXGI_SWAP_CHAIN_DESC swapChainDescription;
    memset( &swapChainDescription, 0, sizeof(swapChainDescription) );
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
    swapChainDescription.BufferCount = 2;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescription.Flags = 0;
    swapChainDescription.OutputWindow = static_cast<HWND>(window->m_Hwnd);
    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.Windowed = TRUE;
    swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.BufferDesc.Width = window->GetClientWidth();
    swapChainDescription.BufferDesc.Height = window->GetClientHeight();

    // Instance - singleton
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags, // controls the type of device we make
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDescription,
        OUT &swapChain,
        OUT &m_Device,
        nullptr,
        OUT &m_Context );

    GUARANTEE_OR_DIE( SUCCEEDED( result ), "Failed to create rendering device" );

    m_SwapChain = new SwapChain( this, swapChain );

    CreateDefaultShaders();
    CreateBuiltInTextures();
    CreateBuiltInSamplers();

    m_DefaultRasterizer = CreateOrGetRasterizer( CullMode::BACK, FillMode::TRIANGLE,
                                                 WindOrder::COUNTER_CLOCKWISE );

    CreateBlendStates();

    m_TextureBuffer = new TextureBuffer( this );
    m_NormalBuffer = new TextureBuffer( this );

    m_FrameUBO = new ConstantBuffer<FrameDataUBO>( this );
    m_CameraUBO = new ConstantBuffer<CameraDataUBO>( this);
    m_ModelUBO = new ConstantBuffer<ModelDataUBO>( this );
    m_LightUBO = new ConstantBuffer<LightDataUBO>( this );
    m_FogUBO = new ConstantBuffer<FogDataUBO>( this );

    m_VertexBuffer = new VertexBuffer<Vertex_PCU>( this, RenderMemoryHint::MEMORY_HINT_DYNAMIC);
    m_IndexBuffer = new IndexBuffer( this, RenderMemoryHint::MEMORY_HINT_DYNAMIC);


    m_EffectCamera = new Camera( this );
}

//-----------------------------------------------------------------------------
void RenderContext::BeginFrame()
{
}

//-------------------------------------------------------------------------------
void RenderContext::EndFrame()
{
    m_SwapChain->Present();
}

//-------------------------------------------------------------------------------
void RenderContext::Shutdown()
{
    std::map<const std::string, Shader*>::iterator shaderIterator;
    for ( shaderIterator = m_LoadedShaders.begin(); shaderIterator != m_LoadedShaders.end(); ++
          shaderIterator )
    {
        Shader* shaderToDelete = shaderIterator->second;
        if ( shaderToDelete != nullptr )
        {
            delete shaderToDelete;
            shaderToDelete = nullptr;
        }
    }

    std::map<const std::string, ShaderProgram*>::iterator shaderProgramIterator;
    for ( shaderProgramIterator = m_LoadedShaderPrograms.begin(); shaderProgramIterator != m_LoadedShaderPrograms.end(); ++
          shaderProgramIterator )
    {
        ShaderProgram* shaderProgramToDelete = shaderProgramIterator->second;
        if ( shaderProgramToDelete != nullptr )
        {
            delete shaderProgramToDelete;
            shaderProgramToDelete = nullptr;
        }
    }

    std::map<const std::string, Sampler*>::iterator samplerIndex;
    for ( samplerIndex = m_LoadedSamplers.begin(); samplerIndex != m_LoadedSamplers.end(); ++
          samplerIndex )
    {
        Sampler* samplerToDelete = samplerIndex->second;
        if ( samplerToDelete != nullptr )
        {
            delete samplerToDelete;
            samplerToDelete = nullptr;
        }
    }

    // Error texture is assigned to multiple textures that fail to load
    Texture*& errorTexture = m_LoadedTextures.at( "ERROR_TEXTURE" );
    GUARANTEE_OR_DIE( errorTexture, "RenderContext::CreateOrGetTextureFromFile - Builtin error texture does not exist" );
    std::map<const std::string, Texture*>::iterator textureIterator;
    for ( textureIterator = m_LoadedTextures.begin(); textureIterator != m_LoadedTextures.end(); ++
          textureIterator )
    {
        Texture*& textureToDelete = textureIterator->second;
        // Handle deleting the error texture separately
        if ( textureToDelete != nullptr && textureToDelete != errorTexture )
        {
            delete textureToDelete;
            textureToDelete = nullptr;
        }
    }
    // Delete the error texture
    delete errorTexture;
    errorTexture = nullptr;


    std::map<const std::string, BitmapFont*>::iterator bitmapIterator;
    for ( bitmapIterator = m_LoadedBitmapFonts.begin(); bitmapIterator != m_LoadedBitmapFonts.end();
          ++bitmapIterator )
    {
        BitmapFont* bitmapFontToDelete = bitmapIterator->second;
        if ( bitmapFontToDelete != nullptr )
        {
            delete bitmapFontToDelete;
            bitmapFontToDelete = nullptr;
        }
    }

    std::map<const std::string, SpriteSheet*>::iterator spriteIterator;
    for ( spriteIterator = m_LoadedSpriteSheets.begin(); spriteIterator != m_LoadedSpriteSheets.
          end(); ++spriteIterator )
    {
        SpriteSheet* spriteSheetToDelete = spriteIterator->second;
        if ( spriteSheetToDelete != nullptr )
        {
            delete spriteSheetToDelete;
            spriteSheetToDelete = nullptr;
        }
    }

    std::map<const std::string, MaterialSheet*>::iterator materialIterator;
    for( materialIterator = m_LoadedMaterialSheets.begin(); materialIterator != m_LoadedMaterialSheets.
        end(); ++materialIterator )
    {
        MaterialSheet* materialSheetToDelete = materialIterator->second;
        if( materialSheetToDelete != nullptr )
        {
            delete materialSheetToDelete;
            materialSheetToDelete = nullptr;
        }
    }

    for ( Rasterizer*& rasterizer : m_LoadedRasterizers )
    {
        if ( rasterizer != nullptr )
        {
            delete rasterizer;
            rasterizer = nullptr;
        }
    }

    GUARANTEE_OR_DIE( m_TexturePool.size() == m_NumTexturesInPool, "Failed to release all textures in texture pool" );
    for( Texture*& texture : m_TexturePool )
    {
        if( texture != nullptr )
        {
            delete texture;
            texture = nullptr;
        }
    }

    if( m_EffectCamera != nullptr )
    {
        delete m_EffectCamera;
        m_EffectCamera = nullptr;
    }

    if ( m_SwapChain != nullptr )
    {
        delete m_SwapChain;
        m_SwapChain = nullptr;
    }

    if( m_TextureBuffer != nullptr )
    {
        delete m_TextureBuffer;
        m_TextureBuffer = nullptr;
    }

    if( m_NormalBuffer != nullptr )
    {
        delete m_NormalBuffer;
        m_NormalBuffer = nullptr;
    }

    if ( m_FrameUBO != nullptr )
    {
        delete m_FrameUBO;
        m_FrameUBO = nullptr;
    }

    if( m_LightUBO != nullptr )
    {
        delete m_LightUBO;
        m_LightUBO = nullptr;
    }

    if ( m_CameraUBO != nullptr )
    {
        delete m_CameraUBO;
        m_CameraUBO = nullptr;
    }

    if ( m_ModelUBO != nullptr )
    {
        delete m_ModelUBO;
        m_ModelUBO = nullptr;
    }

    if ( m_FogUBO != nullptr )
    {
        delete m_FogUBO;
        m_FogUBO = nullptr;
    }

    if ( m_VertexBuffer != nullptr )
    {
        delete m_VertexBuffer;
        m_VertexBuffer = nullptr;
    }

    if ( m_IndexBuffer != nullptr )
    {
        delete m_IndexBuffer;
        m_IndexBuffer = nullptr;
    }

    DX_SAFE_RELEASE( m_CurrentDepthStencil );
    DX_SAFE_RELEASE( m_AlphaBlendState );
    DX_SAFE_RELEASE( m_AdditiveBlendState );
    DX_SAFE_RELEASE( m_SolidBlendState );
    DX_SAFE_RELEASE( m_Context );
    DX_SAFE_RELEASE( m_Device );

#if defined(RENDER_DEBUG)
    ReportLiveObjects();
    DestroyDebugModule();
#endif
}

IntVec2 RenderContext::GetSwapChainSize() const
{
    return m_SwapChain->GetBackBuffer()->GetTextureSize();
}

Texture* RenderContext::GetBackBuffer() const
{
    return m_SwapChain->GetBackBuffer();
}

//-------------------------------------------------------------------------------
void RenderContext::UpdateFrameConstants( float deltaSeconds, const Clock* gameClock,
                                          const float gamma )
{
    ConstantBuffer<FrameDataUBO>* frameData = dynamic_cast<ConstantBuffer<FrameDataUBO>*>(m_FrameUBO);
    FrameDataUBO data = frameData->GetLocalBuffer();
    data.systemTime = static_cast<float>(GetCurrentTimeSeconds());
    data.systemDeltaTime = deltaSeconds;
    data.gameTime = static_cast<float>(gameClock->GetTotalElapsedSeconds());
    data.gameDeltaTime = static_cast<float>(gameClock->GetLastDeltaSeconds());

    data.gamma = gamma;
    data.inverseGamma = 1.f / gamma;

    frameData->UpdateLocalBuffer( data );
}

//-------------------------------------------------------------------------------
void RenderContext::ClearColor( Camera& camera ) 
{
    static float clearFloats[ 4 ];

    for( size_t index = 0; index < camera.GetColorTargetCount(); ++index )
    {
        const Rgba8 clearColor = camera.GetClearColor( index );

        clearFloats[ 0 ] = clearColor.GetRedAsFloat();
        clearFloats[ 1 ] = clearColor.GetGreenAsFloat();
        clearFloats[ 2 ] = clearColor.GetBlueAsFloat();
        clearFloats[ 3 ] = clearColor.GetAlphaAsFloat();

        Texture* colorTarget = camera.GetColorTarget( index );
        TextureView* colorTargetView = colorTarget->CreateOrGetRenderTargetView();
        
        ID3D11RenderTargetView* rtv = colorTargetView->GetAsRTV();
        m_Context->ClearRenderTargetView( rtv, clearFloats );
    }
}

void RenderContext::ClearDepth( Texture* depthTexture, const float depth )
{
    TextureView* view = depthTexture->CreateOrGetDepthStencilView();
    ID3D11DepthStencilView* dsv = view->GetAsDSV();
    m_Context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH, depth, 0 );
}

//-------------------------------------------------------------------------------
void RenderContext::BeginCamera( Camera& camera )
{
    m_CurrentCamera = &camera;

#if defined(RENDER_DEBUG)
    ClearAndRebindState();
#endif

    Texture* colorTarget = camera.GetColorTarget();
    GUARANTEE_OR_DIE( colorTarget != nullptr, "RenderContext::BeginCamera - Camera must have a color target " );
    
    // Create Multiple Render Targets
    std::vector<ID3D11RenderTargetView*> renderTargetViews;
    const size_t colorTargetSize = camera.GetColorTargetCount();
    renderTargetViews.resize( colorTargetSize );

    for( size_t index = 0; index < colorTargetSize; ++index )
    {
        Texture* currentTarget = camera.GetColorTarget( index );
        if( currentTarget != nullptr )
        {
            TextureView* view = nullptr;
            view = currentTarget->CreateOrGetRenderTargetView();
            renderTargetViews[ index ] = view->GetAsRTV();
        }
        else
        {
            renderTargetViews[ index ] = nullptr;
        }
    }

    // Create a Depth Stencil View
    Texture* depthTexture = camera.GetDepthTexture();
    ID3D11DepthStencilView* dsv = nullptr;
    if ( depthTexture != nullptr )
    {
        dsv = depthTexture->CreateOrGetDepthStencilView()->GetAsDSV();
    }

    // Bind Render Targets
    m_Context->OMSetRenderTargets( static_cast<unsigned int>(colorTargetSize), 
                                   renderTargetViews.data(), dsv );

    // Setup a viewport 
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = camera.GetColorTarget()->GetWidth();
    viewport.Height = camera.GetColorTarget()->GetHeight();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_Context->RSSetViewports( 1, &viewport );

    // Clear with camera settings
    if ( camera.ShouldClearColor() )
    {
        ClearColor( camera );
    }
    if ( camera.ShouldClearDepth() )
    {
        ClearDepth( camera.GetDepthTexture(), camera.GetClearDepth() );
        SetDepthTest( DepthCompare::LESS, true );
    }

    // Set up renderer and Camera Buffer
    m_DuringDrawing = true;
    m_Context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    ConstantBuffer<CameraDataUBO>* cameraData = dynamic_cast<ConstantBuffer<CameraDataUBO>*>(m_CameraUBO);
    cameraData->UpdateLocalBuffer( camera.GetCameraData() );

    // Reset Renderer state to default
    m_VertexBuffer->Flush();
    m_IndexBuffer->Flush();
    BindRasterizer( nullptr );
    BindShader( nullptr );
    BindSampler( 0, nullptr );
    BindTexture( nullptr );
    BindNormalMap( nullptr );
    SetModelUBO();
    SetBlendMode( BlendMode::ALPHA );
    SetAmbientLight( Rgba8::WHITE, 1.f );
    DisableFog();
    m_CurrentLayout = nullptr;
}

//-----------------------------------------------------------------------------
void RenderContext::EndCamera( const Camera& camera )
{
    DrawIfPending();

    m_DuringDrawing = false;

    UNUSED( camera );
    m_CurrentCamera = nullptr;
}

Texture* RenderContext::CreateOrGetTextureFromFile( const std::string& filePath, bool flipV )
{
    if ( m_LoadedTextures.find( filePath ) != m_LoadedTextures.end() )
    {
        return m_LoadedTextures.at( filePath );
    }
    Texture* newlyCreatedTexture = Texture::CreateFromFile( this, filePath, flipV );
    if( newlyCreatedTexture == nullptr )
    {
#if !defined(ENGINE_DISABLE_CONSOLE)
        g_Console->Log( LOG_ERROR, Stringf( "RenderContext::CreateOrGetTextureFromFile - Failed to load %s. Using builtin error texture", filePath.c_str() ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)

        newlyCreatedTexture = m_LoadedTextures.at( "ERROR_TEXTURE" );

        GUARANTEE_OR_DIE( newlyCreatedTexture, "RenderContext::CreateOrGetTextureFromFile - Builtin error texture does not exist" );
    }
    m_LoadedTextures[ filePath ] = newlyCreatedTexture;

    return newlyCreatedTexture;
}

Sampler* RenderContext::CreateOrGetSamplerFromFile( const std::string& filePath )
{
    if ( m_LoadedSamplers.find( filePath ) != m_LoadedSamplers.cend() )
    {
        return m_LoadedSamplers.at( filePath );
    }
    Sampler* newlyCreatedSampler = CreateSamplerFromFile( filePath );
    m_LoadedSamplers[ filePath ] = newlyCreatedSampler;

    return newlyCreatedSampler;
}

Shader* RenderContext::CreateOrGetShaderFromFile( const std::string& filePath )
{
    if ( m_LoadedShaders.find( filePath ) != m_LoadedShaders.cend() )
    {
        return m_LoadedShaders.at( filePath );
    }
    Shader* newlyCreatedShader = CreateShaderFromFile( filePath );
    m_LoadedShaders[ filePath ] = newlyCreatedShader;

    return newlyCreatedShader;
}


ShaderProgram* RenderContext::CreateOrGetShaderProgramFromFile( const std::string& filePath )
{
    if ( m_LoadedShaderPrograms.find( filePath ) != m_LoadedShaderPrograms.cend() )
    {
        return m_LoadedShaderPrograms.at( filePath );
    }
    ShaderProgram* newlyCreatedShaderProgram = CreateShaderProgramFromFile( filePath );
    m_LoadedShaderPrograms[ filePath ] = newlyCreatedShaderProgram;

    return newlyCreatedShaderProgram;
}

SpriteSheet* RenderContext::CreateOrGetSpriteSheetFromFile( const std::string& filePath,
                                                            const IntVec2& size, bool flipV )
{
    if ( m_LoadedSpriteSheets.find( filePath ) != m_LoadedSpriteSheets.end() )
    {
        return m_LoadedSpriteSheets.at( filePath );
    }
    SpriteSheet* newlyCreatedSpriteSheet = CreateSpriteSheetFromFile( filePath, size, flipV );
    m_LoadedSpriteSheets[ filePath ] = newlyCreatedSpriteSheet;

    return newlyCreatedSpriteSheet;
}

Rasterizer* RenderContext::CreateOrGetRasterizer( const CullMode cullMode, const FillMode fillMode,
                                                  const WindOrder order )
{
    for ( Rasterizer* rasterizer : m_LoadedRasterizers )
    {
        if ( rasterizer->Equals( cullMode, fillMode, order ) )
        {
            return rasterizer;
        }
    }

    Rasterizer* rasterizer = new Rasterizer( this, cullMode, fillMode, order );
    m_LoadedRasterizers.push_back( rasterizer );

    return rasterizer;
}

MaterialSheet* RenderContext::CreateOrGetMaterialSheetFromData( const std::string& name, const std::map<std::string, SpriteSheet*> data, const IntVec2& gridLayout )
{
    if( m_LoadedMaterialSheets.find( name )!= m_LoadedMaterialSheets.end() )
    {
        return m_LoadedMaterialSheets.at( name );
    }
    MaterialSheet* newlyCreatedMaterialSheet = CreateMaterialSheetFromData( data, gridLayout );
    m_LoadedMaterialSheets[ name ] = newlyCreatedMaterialSheet;
    return newlyCreatedMaterialSheet;
}

MaterialSheet* RenderContext::GetMaterialSheet( const std::string& name )
{
    const auto foundMaterialSheet = m_LoadedMaterialSheets.find( name );
    if( foundMaterialSheet != m_LoadedMaterialSheets.cend() )
    {
        return foundMaterialSheet->second;
    }
    return nullptr;
}

Texture* RenderContext::AcquireMatchingRenderTarget( Texture* texture )
{
    const IntVec2 size = texture->GetTextureSize();

    for( size_t index = 0; index < m_TexturePool.size(); ++index )
    {
        Texture* renderTexture = m_TexturePool.at( index );
        if( size == renderTexture->GetTextureSize() )
        {
            m_TexturePool[ index ] = m_TexturePool[ m_TexturePool.size() - 1 ];
            m_TexturePool.pop_back();

            return renderTexture;
        }
    }

    m_NumTexturesInPool++;

    GUARANTEE_OR_DIE( m_NumTexturesInPool < 256, "RenderContext::AcquireMatchingRenderTarget - Too many texture to allocate" );

    Texture* renderTarget = Texture::CreateRenderTargetFromSize( this, size );
    return renderTarget;
}

void RenderContext::ReleaseRenderTarget( Texture* texture )
{
    m_TexturePool.push_back( texture );
}

size_t RenderContext::GetTotalTexturePoolCount() const
{
    return m_TexturePool.size();
}

size_t RenderContext::GetFreeTexturePoolCount() const
{
    return m_NumTexturesInPool;
}

void RenderContext::StartEffect( Texture* dst, Texture* src, ShaderProgram* shader )
{
    m_EffectCamera->SetColorTarget( dst );
    BeginCamera( *m_EffectCamera );

    m_EffectShader = new Shader( shader );
    m_EffectShader->depthCompare = DepthCompare::ALWAYS;
    m_EffectShader->writeDepth = false;
    m_EffectShader->blendMode = BlendMode::ALPHA;

    BindShader( m_EffectShader );
    BindTexture( src );
}

void RenderContext::StartEffect( Texture* dst, Texture* src, Material* material )
{
    m_EffectCamera->SetColorTarget( dst );
    BeginCamera( *m_EffectCamera );

    BindMaterial( material );
    BindTexture( src );
}

void RenderContext::EndEffect()
{
    Finalize();
    m_Context->Draw( 3, 0 );
    BindShader( nullptr );

    delete m_EffectShader;
    EndCamera( *m_EffectCamera );
}

BitmapFont* RenderContext::CreateOrGetBitmapFontFromFile( const std::string& filePath )
{
    if ( m_LoadedBitmapFonts.find( filePath ) != m_LoadedBitmapFonts.cend() )
    {
        return m_LoadedBitmapFonts.at( filePath );
    }
    static std::string fileExten = ".png";
    BitmapFont* newlyCreatedBitmapFont = CreateBitmapFromFile( filePath + fileExten );

    m_LoadedBitmapFonts[ filePath ] = newlyCreatedBitmapFont;
    return newlyCreatedBitmapFont;
}

void RenderContext::SetBlendMode( BlendMode blendMode )
{
    const float zeros[] = {0.f, 0.f, 0.f, 0.f};

    switch ( blendMode )
    {
    case BlendMode::ALPHA:
        m_Context->OMSetBlendState( m_AlphaBlendState, zeros, static_cast<UINT>(~0) );
        break;
    case BlendMode::SOLID:
        m_Context->OMSetBlendState( m_SolidBlendState, zeros, static_cast<UINT>(~0) );
    case BlendMode::ADDITIVE:
        m_Context->OMSetBlendState( m_AdditiveBlendState, zeros, static_cast<UINT>(~0) );
        break;
    default:
        ERROR_AND_DIE( Stringf( "Unknown / unsupported bledn mode #%i", blendMode ) );
        break;
    }
}

void RenderContext::BindMaterial( const Material* constMaterial )
{
    DrawIfPending();

    Material* material = const_cast<Material*>(constMaterial);

    if( material->m_ModelData->IsDirty() )
    {
        material->m_ModelData->UpdateAndBind( UBO_OBJECT_SLOT );
    }

    if( material->m_DiffuseTexture->IsDirty() )
    {
        material->m_DiffuseTexture->UpdateAndBindBuffer( TEXTURE_DIFFUSE_SLOT );
    }

    if( material->m_NormalTexture->IsDirty() )
    {
        material->m_DiffuseTexture->UpdateAndBindBuffer( TEXTURE_NORMAL_SLOT );
    }

    for( int bufferIndex = 0; bufferIndex < 8; ++bufferIndex )
    {
        RenderBuffer*& constantBuffer = material->m_ConstantBuffers[ bufferIndex ];
        if( constantBuffer != nullptr )
        {
            constantBuffer->UpdateAndBind( bufferIndex + 8 );
        }

        TextureBuffer*& textureBuffer = material->m_TextureBuffers[ bufferIndex ];
        if( textureBuffer != nullptr )
        {
            textureBuffer->UpdateAndBindBuffer( bufferIndex + 8 );
        }

        if( material->m_Samplers[ bufferIndex] != nullptr )
        {
            BindSampler( bufferIndex + 8, material->m_Samplers[ bufferIndex ] );
        }
    }

    BindShader( material->m_Shader );
}

//-----------------------------------------------------------------------------
void RenderContext::BindTexture( const Texture* constTexture )
{
    DrawIfPending();
    
    Texture* texture = const_cast<Texture*>(constTexture);
    if ( texture == nullptr )
    {
        texture = m_LoadedTextures["WHITE"];
    }

    m_TextureBuffer->UpdateLocalBuffer( texture );
}

void RenderContext::BindNormalMap( const Texture* constTexture )
{
    DrawIfPending();

    Texture* texture = const_cast<Texture*>(constTexture);
    if ( texture == nullptr )
    {
        texture = m_LoadedTextures["FLAT"];
    }

    m_NormalBuffer->UpdateLocalBuffer( texture );
}

void RenderContext::BindSampler( unsigned int index, const Sampler* constSampler )
{
    DrawIfPending();

    Sampler* sampler = const_cast<Sampler*>(constSampler);
    if ( sampler == nullptr && index == 0)
    {
        sampler = m_LoadedSamplers["LINEAR"];
    }

    ID3D11SamplerState* samplerHandle = sampler->GetHandle();
    m_Context->PSSetSamplers( index, 1, &samplerHandle );
}

void RenderContext::BindSampler( const std::string& fileName )
{
    BindSampler( 0, CreateOrGetSamplerFromFile( fileName ) );
}

void RenderContext::BindRasterizer( const Rasterizer* rasterizer )
{
    const Rasterizer* newRasterizer = rasterizer;
    if ( newRasterizer == nullptr )
    {
        newRasterizer = m_DefaultRasterizer;
    }

    if ( newRasterizer != m_CurrentRasterizer )
    {
        m_Context->RSSetState( newRasterizer->GetRasterState() );
        m_CurrentRasterizer = newRasterizer;
    }
}

void RenderContext::SetRasterizer( CullMode cullMode, FillMode fillMode, WindOrder order )
{
    BindRasterizer( CreateOrGetRasterizer( cullMode, fillMode, order ) );
}

void RenderContext::BindUniformBuffer( unsigned int slot, RenderBuffer* ubo ) const
{
    ID3D11Buffer* uboHandle = ubo->m_Handle;

    m_Context->VSSetConstantBuffers( slot, 1, &uboHandle );
    m_Context->PSSetConstantBuffers( slot, 1, &uboHandle );
}

void RenderContext::BindShader( const Shader* shader )
{
    DrawIfPending();

    if( shader == nullptr )
    {
        shader = m_DefaultShader;
    }

    if( *m_CurrentShader != *shader )
    {
        m_ShaderIsDirty = true;
        *m_CurrentShader = *shader;
    }
}

void RenderContext::BindShader( const std::string& fileName )
{
    BindShader( CreateOrGetShaderFromFile( fileName ) );
}

void RenderContext::SetModelUBO( const Mat44& modelMatrix, const Rgba8& modelTint, const float specularFactor, const float specularPower )
{
    DrawIfPending();

    ConstantBuffer<ModelDataUBO>* modelData = dynamic_cast<ConstantBuffer<ModelDataUBO>*>(m_ModelUBO);
    ModelDataUBO model = modelData->GetLocalBuffer();
    model.meshModel = modelMatrix;
    model.meshTint = modelTint.GetAsVec4();
    model.specularFactor = specularFactor;
    model.specularPower = specularPower;

    modelData->UpdateLocalBuffer( model );
}

// bool RenderContext::GetDepthTest( DepthCompare& compareFunction ) const
// {
//     D3D11_DEPTH_STENCIL_DESC desc;
//     m_CurrentDepthStencil->GetDesc( &desc );
//
//     switch ( desc.DepthFunc )
//     {
//     case D3D11_COMPARISON_NEVER: compareFunction = DepthCompare::NEVER;
//         break;
//     case D3D11_COMPARISON_ALWAYS: compareFunction = DepthCompare::ALWAYS;
//         break;
//     case D3D11_COMPARISON_EQUAL: compareFunction = DepthCompare::EQUAL;
//         break;
//     case D3D11_COMPARISON_GREATER: compareFunction = DepthCompare::GREATER;
//         break;
//     case D3D11_COMPARISON_LESS: compareFunction = DepthCompare::LESS;
//         break;
//     case D3D11_COMPARISON_LESS_EQUAL: compareFunction = DepthCompare::LESS_EQUAL;
//         break;
//     default: ERROR_AND_DIE( "Depth function is not a supported type" );
//         break;
//     }
//
//     if ( desc.DepthWriteMask == D3D11_DEPTH_WRITE_MASK_ALL )
//     {
//         return true;
//     }
//     return false;
// }

void RenderContext::SetDepthTest( DepthCompare compareFunction, bool writeDepthOnPass )
{
    if ( !CheckDepthState( compareFunction, writeDepthOnPass ) )
    {
        DX_SAFE_RELEASE( m_CurrentDepthStencil );

        D3D11_DEPTH_STENCIL_DESC desc;
        memset( &desc, 0, sizeof(desc) );

        desc.DepthEnable = true;
        switch ( compareFunction )
        {
        case DepthCompare::NEVER: desc.DepthFunc = D3D11_COMPARISON_NEVER;
            break;
        case DepthCompare::ALWAYS: desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
            break;
        case DepthCompare::EQUAL: desc.DepthFunc = D3D11_COMPARISON_EQUAL;
            break;
        case DepthCompare::GREATER: desc.DepthFunc = D3D11_COMPARISON_GREATER;
            break;
        case DepthCompare::LESS: desc.DepthFunc = D3D11_COMPARISON_LESS;
            break;
        case DepthCompare::LESS_EQUAL: desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            break;
        default: desc.DepthFunc = D3D11_COMPARISON_LESS;
            break;
        }
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        if ( writeDepthOnPass )
        {
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        }

        m_Device->CreateDepthStencilState( &desc, &m_CurrentDepthStencil );
    }

    m_Context->OMSetDepthStencilState( m_CurrentDepthStencil, 0U );
}

// void RenderContext::SetCullMode( const CullMode cullMode )
// {
//     SetRasterizer( cullMode, m_CurrentRasterizer->GetFillMode(),
//                     m_CurrentRasterizer->GetWindOrder() );
// }
//
// void RenderContext::SetFillMode( const FillMode fillMode )
// {
//     SetRasterizer( m_CurrentRasterizer->GetCullMode(), fillMode,
//                     m_CurrentRasterizer->GetWindOrder() );
// }
//
// void RenderContext::SetWindOrder( const WindOrder windOrder )
// {
//     SetRasterizer( m_CurrentRasterizer->GetCullMode(), m_CurrentRasterizer->GetFillMode(),
//                     windOrder );
// }

void RenderContext::SetAmbientLight( const Rgba8& color, float intensity )
{
    ConstantBuffer<LightDataUBO>* lightData = dynamic_cast<ConstantBuffer<LightDataUBO>*>(m_LightUBO);
    LightDataUBO copy = lightData->GetLocalBuffer();
    copy.ambient = Vec4( color.GetAsVec4().XYZ(), intensity );
    
    lightData->UpdateLocalBuffer( copy );
}

void RenderContext::EnableLight( size_t index, const Light& light )
{
    GUARANTEE_OR_DIE( index < 8,
                      Stringf(
                          "EnableLight: Index %u out of bounds. Only 8 (0-7) available light indexes"
                      ) );
    ConstantBuffer<LightDataUBO>* lightData = dynamic_cast<ConstantBuffer<LightDataUBO>*>(m_LightUBO);
    LightDataUBO copy = lightData->GetLocalBuffer();
    copy.lights[ index ] = light;
    
    lightData->UpdateLocalBuffer( copy );
}

void RenderContext::DisableLight( size_t index )
{
    GUARANTEE_OR_DIE( index < 8,
                      Stringf(
                          "DisableLight: Index %u out of bounds. Only 8 (0-7) available light indexes"
                          ) );

    // Sets the intensity of the light to zero (disabled)
    ConstantBuffer<LightDataUBO>* lightData = dynamic_cast<ConstantBuffer<LightDataUBO>*>(m_LightUBO);
    LightDataUBO copy = lightData->GetLocalBuffer();
    copy.lights[ index ].color.w = 0.f;

    lightData->UpdateLocalBuffer( copy );
}

void RenderContext::SetFogData( const FogDataUBO& fogDataUBO )
{
    ConstantBuffer<FogDataUBO>* fogData = dynamic_cast<ConstantBuffer<FogDataUBO>*>(m_FogUBO);
    fogData->UpdateLocalBuffer( fogDataUBO );
}

void RenderContext::DisableFog()
{
    ConstantBuffer<FogDataUBO>* fogData = dynamic_cast<ConstantBuffer<FogDataUBO>*>(m_FogUBO);

    FogDataUBO fogDataUBO = fogData->GetLocalBuffer();
fogDataUBO.farFogDistance = INFINITY;
    fogDataUBO.nearFogDistance = INFINITY;
    fogData->UpdateLocalBuffer( fogDataUBO );
}

bool RenderContext::CheckDepthState( const DepthCompare compareFunction,
                                     const bool writeDepthOnPass )
{
    if ( m_CurrentDepthStencil == nullptr )
    {
        return false;
    }

    D3D11_COMPARISON_FUNC func;
    switch ( compareFunction )
    {
    case DepthCompare::NEVER: func = D3D11_COMPARISON_NEVER;
        break;
    case DepthCompare::ALWAYS: func = D3D11_COMPARISON_ALWAYS;
        break;
    case DepthCompare::EQUAL: func = D3D11_COMPARISON_EQUAL;
        break;
    case DepthCompare::GREATER: func = D3D11_COMPARISON_GREATER;
        break;
    case DepthCompare::LESS: func = D3D11_COMPARISON_LESS;
        break;
    default: func = D3D11_COMPARISON_LESS;
        break;
    }
    D3D11_DEPTH_WRITE_MASK mask = D3D11_DEPTH_WRITE_MASK_ZERO;
    if ( writeDepthOnPass )
    {
        mask = D3D11_DEPTH_WRITE_MASK_ALL;
    }

    D3D11_DEPTH_STENCIL_DESC desc;
    m_CurrentDepthStencil->GetDesc( &desc );

    return desc.DepthEnable && desc.DepthFunc == func && desc.DepthWriteMask == mask;
}

void RenderContext::DrawVertexArray( const std::vector<VertexMaster>& vertexes )
{
    const size_t vectorSize = vertexes.size();
    if ( vectorSize > 0 )
    {
        // TODO: Fix up vertex_pcu
        std::vector<Vertex_PCU> pcu;
        Vertex_PCU::ConvertFromMaster( pcu, vertexes );
        DrawVertexArray( vectorSize, &pcu[ 0 ] );
    }
}

void RenderContext::CopyTexture( Texture* des, Texture* src ) const
{
    m_Context->CopyResource( des->GetHandle(), src->GetHandle() );
}

void RenderContext::SaveShaderState( OUT_PARAM Shader* copyTo ) const
{
    *copyTo = *m_CurrentShader;
}

void RenderContext::LoadShaderState( const Shader* copyFrom )
{
    DrawIfPending();

    *m_CurrentShader = *copyFrom;
    BindShader();
}

void RenderContext::ReloadShaders()
{
    std::map<const std::string, ShaderProgram*>::iterator shaderIterator;
    for ( shaderIterator = m_LoadedShaderPrograms.begin(); shaderIterator != m_LoadedShaderPrograms.end(); ++
          shaderIterator )
    {
        if( shaderIterator->second != nullptr )
        {
            shaderIterator->second->Reload();
        }
    }
}

//-----------------------------------------------------------------------------
void RenderContext::DrawVertexArray( size_t numVertexes, const Vertex_PCU* vertexes )
{
    UpdateLayoutIfNeeded( Vertex_PCU::LAYOUT );

    VertexBuffer<Vertex_PCU>* vertexBuffer = VertexBuffer<Vertex_PCU>::FromRenderBuffer( m_VertexBuffer );
    const size_t bufferStart = vertexBuffer->AppendLocalBuffer( vertexes, static_cast<unsigned int>(numVertexes) );

    m_IndexBuffer->AppendLocalBuffer( numVertexes, bufferStart );
}

void RenderContext::DrawMesh( GPUMesh* mesh )
{
    UpdateLayoutIfNeeded( mesh->GetVertexBuffer()->m_BufferAttribute );
    Finalize();

    mesh->GetVertexBuffer()->UpdateAndBind( 0 );


    const bool usesIndexes = mesh->GetIndexCount() > 0;

    if ( usesIndexes )
    {
        mesh->GetIndexBuffer()->UpdateAndBind( );

        m_Context->DrawIndexed( static_cast<UINT>(mesh->GetIndexCount()), 0, 0 );
    }
    else
    {
        Draw( mesh->GetVertexCount(), 0 );
    }
}

void RenderContext::DrawIndexed( std::vector<VertexMaster>& vertexes,
                                 std::vector<unsigned int>& indexes )
{
    // TODO: Fix up vertex_pcu
    std::vector<Vertex_PCU> pcu;
    Vertex_PCU::ConvertFromMaster( pcu, vertexes );

    UpdateLayoutIfNeeded( Vertex_PCU::LAYOUT );

    VertexBuffer<Vertex_PCU>* vertexBuffer = VertexBuffer<Vertex_PCU>::FromRenderBuffer( m_VertexBuffer );
    const size_t bufferStart = vertexBuffer->AppendLocalBuffer( pcu );
    m_IndexBuffer->AppendLocalBuffer( indexes, bufferStart );
}

void RenderContext::Draw( size_t numVertexes, size_t vertexOffset )
{
    Finalize();
    
    m_Context->Draw( static_cast<unsigned int>(numVertexes),
                     static_cast<unsigned int>(vertexOffset) );
}


Sampler* RenderContext::CreateSamplerFromFile( const std::string& filePath )
{
    XmlDocument doc;
    doc.LoadFile( filePath.c_str() );
    Sampler* newSampler = Sampler::PopulateFromXmlElement( this, *doc.RootElement() );
    return newSampler;
}

Shader* RenderContext::CreateShaderFromFile( const std::string& filePath )
{
    XmlDocument doc;
    doc.LoadFile( filePath.c_str() );
    Shader* newShader = Shader::PopulateFromXmlElement( this, *doc.RootElement() );
    return newShader;
}

ShaderProgram* RenderContext::CreateShaderProgramFromFile( const std::string& filePath )
{
    ShaderProgram* newShader = new ShaderProgram( this );
    newShader->CreateFromFile( filePath );

    return newShader;
}

BitmapFont* RenderContext::CreateBitmapFromFile( const std::string& filePath )
{
    BitmapFont* newBitmap = new BitmapFont( filePath.c_str(),
                                            CreateOrGetTextureFromFile( filePath ) );
    return newBitmap;
}

SpriteSheet* RenderContext::CreateSpriteSheetFromFile( const std::string& filePath, const IntVec2& size, bool flipV )
{
    SpriteSheet* newSheet = new SpriteSheet( *CreateOrGetTextureFromFile( filePath, flipV ), size, flipV );
    return newSheet;
}

MaterialSheet* RenderContext::CreateMaterialSheetFromData( const std::map<std::string, SpriteSheet*> data, const IntVec2& girdLayout )
{
    MaterialSheet* newSheet = new MaterialSheet( girdLayout );

    for( auto iterator = data.begin(); iterator != data.end(); ++iterator )
    {
        newSheet->AddSpriteSheet( iterator->first, iterator->second );
    }
    return newSheet;
}

void RenderContext::CreateDefaultShaders()
{
    // Create the default shader
    ShaderProgram* defaultShaderProgram = new ShaderProgram( this );
    defaultShaderProgram->CreateFromFile( "Data/Shaders/Default/default.hlsl" );
    m_LoadedShaderPrograms[ "DEFAULT" ] = defaultShaderProgram;
    Shader* defaultShader = new Shader( defaultShaderProgram );
    defaultShader->depthCompare = DepthCompare::ALWAYS;
    defaultShader->writeDepth = false;
    m_LoadedShaders[ "DEFAULT" ] = defaultShader;

    // Create the lit shader
    ShaderProgram* defaultLitProgram = new ShaderProgram( this );
    defaultLitProgram->CreateFromFile( "Data/Shaders/Default/default_lit.hlsl" );
    m_LoadedShaderPrograms[ "DEFAULT_LIT" ] = defaultLitProgram;
    Shader* defaultLit = new Shader( defaultLitProgram );
    defaultLit->writeDepth = true;
    m_LoadedShaders[ "DEFAULT_LIT" ] = defaultLit;

    m_DefaultShader = m_LoadedShaders[ "DEFAULT" ];
    m_CurrentShader = new Shader( nullptr );
    *m_CurrentShader = *m_DefaultShader;
}

void RenderContext::CreateBuiltInTextures()
{
    Texture* textureWhite = Texture::CreateFromColor( this, Rgba8::WHITE );
    m_LoadedTextures[ "WHITE" ] = textureWhite;

    Texture* textureFlat = Texture::CreateFromColor( this, Rgba8( 127, 127, 255 ) );
    m_LoadedTextures[ "FLAT" ] = textureFlat;

    static Rgba8 errorTextureColorArray[] = { Rgba8::YELLOW, Rgba8::MAGENTA, Rgba8::MAGENTA, Rgba8::YELLOW };
    Texture* textureError = Texture::CreateFromColorArray( this, errorTextureColorArray, IntVec2( 2, 2 ) );
    m_LoadedTextures[ "ERROR_TEXTURE" ] = textureError;
}

void RenderContext::CreateBuiltInSamplers()
{
    Sampler* samplePoint = new Sampler( this, SamplerType::POINT, SamplerEdgeMode::CLAMP );
    m_LoadedSamplers[ "POINT" ] = samplePoint;

    Sampler* sampleLinear = new Sampler( this, SamplerType::BILINEAR, SamplerEdgeMode::CLAMP );
    m_LoadedSamplers[ "LINEAR" ] = sampleLinear;
}

void RenderContext::CreateBlendStates()
{
    D3D11_BLEND_DESC descAlpha;
    descAlpha.AlphaToCoverageEnable = false;
    descAlpha.IndependentBlendEnable = false;
    descAlpha.RenderTarget[ 0 ].BlendEnable = true;
    descAlpha.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
    descAlpha.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    descAlpha.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    descAlpha.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    descAlpha.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
    descAlpha.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ONE;
    descAlpha.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    m_Device->CreateBlendState( &descAlpha, &m_AlphaBlendState );

    D3D11_BLEND_DESC descSolid;
    descSolid.AlphaToCoverageEnable = false;
    descSolid.IndependentBlendEnable = false;
    descSolid.RenderTarget[ 0 ].BlendEnable = false;
    descSolid.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
    descSolid.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    descSolid.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    descSolid.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    descSolid.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
    descSolid.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
    descSolid.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    m_Device->CreateBlendState( &descSolid, &m_SolidBlendState );

    D3D11_BLEND_DESC descAdditive;
    descAdditive.AlphaToCoverageEnable = false;
    descAdditive.IndependentBlendEnable = false;
    descAdditive.RenderTarget[ 0 ].BlendEnable = true;
    descAdditive.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
    descAdditive.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_ONE;
    descAdditive.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_ONE;
    descAdditive.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    descAdditive.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
    descAdditive.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
    descAdditive.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    m_Device->CreateBlendState( &descAdditive, &m_AdditiveBlendState );
}

void RenderContext::UpdateLayoutIfNeeded( BufferAttribute* newLayout )
{
    if( newLayout == m_CurrentLayout )
    {
        return;
    }
    else
    {
        DrawIfPending();

        m_CurrentLayout = newLayout;
        UpdateLayout();
        
        if( m_CurrentLayout == Vertex_PCU::LAYOUT )
        {
            if( m_VertexBuffer != nullptr )
            {
                delete m_VertexBuffer;
                m_VertexBuffer = new VertexBuffer<Vertex_PCU>( this, RenderMemoryHint::MEMORY_HINT_DYNAMIC );
            }
        }
        else
        {
            if ( m_VertexBuffer != nullptr )
            {
                delete m_VertexBuffer;
                m_VertexBuffer = new VertexBuffer<Vertex_PCUTBN>( this, RenderMemoryHint::MEMORY_HINT_DYNAMIC );
            }
        }
    }
}

void RenderContext::UpdateLayout() const
{
    ShaderProgram* program = const_cast<ShaderProgram*>(m_CurrentShader->program);

    ID3D11InputLayout* inputLayout = program->CreateOrGetInputLayout( m_CurrentLayout );
    m_Context->IASetInputLayout( inputLayout );
}

void RenderContext::BindShader()
{
    ShaderProgram* program = const_cast<ShaderProgram*>(m_CurrentShader->program);

    m_Context->VSSetShader( program->GetVertexShader()->m_vs, nullptr, 0 );
    m_Context->PSSetShader( program->GetFragmentShader()->m_fs, nullptr, 0 );

    SetBlendMode( m_CurrentShader->blendMode );
    SetDepthTest( m_CurrentShader->depthCompare, m_CurrentShader->writeDepth );
    SetRasterizer( m_CurrentShader->cullMode, m_CurrentShader->fillMode, m_CurrentShader->windOrder );
}

void RenderContext::DrawIfPending()
{
    if( IsDrawPending() )
    {
        Finalize();
        Flush();
    }
}

void RenderContext::Finalize()
{
    if( m_ShaderIsDirty )
    {
        BindShader( );
        m_ShaderIsDirty = false;
    }

    if( m_TextureBuffer->IsDirty() )
    {
        m_TextureBuffer->UpdateAndBindBuffer( TEXTURE_DIFFUSE_SLOT );
    }

    if( m_NormalBuffer->IsDirty() )
    {
        m_NormalBuffer->UpdateAndBindBuffer( TEXTURE_NORMAL_SLOT );
    }

    if( m_FrameUBO->IsDirty() )
    {
        m_FrameUBO->UpdateAndBind( UBO_FRAME_SLOT );
    }

    if( m_LightUBO->IsDirty() )
    {
        m_LightUBO->UpdateAndBind( UBO_LIGHT_SLOT );
    }

    if( m_CameraUBO->IsDirty() )
    {
        m_CameraUBO->UpdateAndBind( UBO_CAMERA_SLOT );
    }

    if( m_ModelUBO->IsDirty() )
    {
        m_ModelUBO->UpdateAndBind( UBO_OBJECT_SLOT );
    }

    if ( m_FogUBO->IsDirty() )
    {
        m_FogUBO->UpdateAndBind( UBO_FOG_SLOT );
    }

    UpdateLayout();
}

void RenderContext::Flush() const
{
    if ( m_VertexBuffer->GetLocalSize() == 0 ) { return; }

    BindVertexBuffer( m_VertexBuffer );
    BindIndexBuffer( m_IndexBuffer );

    // ID3D11InputLayout* inputLayout = m_CurrentShader->CreateOrGetInputLayout( m_CurrentLayout );
    // m_Context->IASetInputLayout( inputLayout );

    m_Context->DrawIndexed( m_IndexBuffer->GetLocalSize(), 0, 0 );

    m_VertexBuffer->Flush();
    m_IndexBuffer->Flush();
}

bool RenderContext::IsDrawPending() const
{
    return m_VertexBuffer->GetLocalSize() > 0;
}

void RenderContext::BindVertexBuffer( RenderBuffer* vbo ) const
{
    if( vbo->IsDirty() )
    {
        vbo->UpdateAndBind( 0 );
    }
}

void RenderContext::BindIndexBuffer( IndexBuffer* ibo ) const
{
    if ( ibo->IsDirty() )
    {
        ibo->UpdateAndBind();
    }
}

void RenderContext::ClearAndRebindState()
{
    m_Context->ClearState();

    ConstantBuffer<CameraDataUBO>* cameraData = ConstantBuffer<CameraDataUBO>::FromRenderBuffer( m_CameraUBO );
    cameraData->UpdateLocalBuffer( m_CurrentCamera->GetCameraData() );

    m_FrameUBO->MakeDirty();
    m_LightUBO->MakeDirty();
    m_CameraUBO->MakeDirty();
    m_ModelUBO->MakeDirty();
    m_ShaderIsDirty = true;

    m_FrameUBO->UpdateAndBind( UBO_FRAME_SLOT );
    m_LightUBO->UpdateAndBind( UBO_LIGHT_SLOT );
    m_CameraUBO->UpdateAndBind( UBO_CAMERA_SLOT );
    m_ModelUBO->UpdateAndBind( UBO_OBJECT_SLOT );

    const Rasterizer* rasterizer = m_CurrentRasterizer != nullptr
                                       ? m_CurrentRasterizer
                                       : m_DefaultRasterizer;
    m_Context->RSSetState( rasterizer->GetRasterState() );
}

//-----------------------------------------------------------------------------
void RenderContext::ReportLiveObjects()
{
    if ( m_Debug != nullptr )
    {
        m_Debug->ReportLiveObjects( DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL );
    }
}

void RenderContext::CreateDebugModule()
{
    m_DebugModule = LoadLibraryA( "Dxgidebug.dll" );
    if ( m_DebugModule == nullptr )
    {
        ERROR_RECOVERABLE( "Failed to find dxgidebug.dll. No debug features enabled." );
    }
    else
    {
        typedef HRESULT ( WINAPI* GetDebugModuleCB )( REFIID, void** );
        GetDebugModuleCB cb = (GetDebugModuleCB)GetProcAddress(
            static_cast<HMODULE>(m_DebugModule), "DXGIGetDebugInterface" );

        HRESULT hr = cb( __uuidof(IDXGIDebug), (void**)&m_Debug );
        GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Debug module failed to initilize." );
    }
}

void RenderContext::DestroyDebugModule()
{
    if ( m_Debug != nullptr )
    {
        DX_SAFE_RELEASE( m_Debug );
        FreeLibrary( static_cast<HMODULE>(m_DebugModule) );

        m_Debug = nullptr;
        m_DebugModule = nullptr;
    }
}
