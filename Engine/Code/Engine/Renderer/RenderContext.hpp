#pragma once

#include "Engine/Core/VertexTypes/Vertex_PCU.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/Math/Primatives/Mat44.hpp"
#include "Engine/Renderer/Sprite/MaterialSheet.hpp"


#include <map>
#include <string>
#include <vector>

#include "Buffers/TextureBuffer.hpp"
#include "Camera.hpp"
#include "D3D11Common.hpp"
#include "Light/Light.hpp"
#include "Rasterizer.hpp"

#include "Shaders/Shader.hpp"

class Material;
//-----------------------------------------------------------------------------
// D3D11 Predefines
struct IDXGIDebug;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11BlendState;
struct ID3D11Resource;
struct ID3D11DepthStencilState;

//-----------------------------------------------------------------------------
// Engine Predefines
struct Rgba8;
struct AABB2;
struct Disc;

class BitmapFont;
class Camera;
class GPUMesh;
class Clock;
class RenderBuffer;
class Sampler;
class ShaderProgram;
class SpriteSheet;
class SwapChain;
class Texture;
class IndexBuffer;
class Window;



enum TextureSlot: unsigned int
{
    TEXTURE_DIFFUSE_SLOT = 0,
    TEXTURE_NORMAL_SLOT = 1,
};

class RenderContext
{
public:
    void* m_DebugModule = nullptr;
    IDXGIDebug* m_Debug = nullptr;

    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_Context = nullptr;

    RenderContext() = default;
    ~RenderContext() = default;

    void Startup( Window* window);
    void BeginFrame();
    void EndFrame();
    void Shutdown();

    IntVec2 GetSwapChainSize() const;
    Texture* GetBackBuffer() const;

    void UpdateFrameConstants( float deltaSeconds, const Clock* gameClock, float gamma );

    void ClearColor( Camera& camera );
    void ClearDepth( Texture* depthTexture, const float depth );
    void BeginCamera( Camera& camera );
    void EndCamera( const Camera& camera );

    Sampler* CreateOrGetSamplerFromFile( const std::string& filePath );
    Shader* CreateOrGetShaderFromFile( const std::string& filePath );
    ShaderProgram* CreateOrGetShaderProgramFromFile( const std::string& filePath );
    Texture* CreateOrGetTextureFromFile( const std::string& filePath, bool flipV = false );
    BitmapFont* CreateOrGetBitmapFontFromFile( const std::string& filePath );
    SpriteSheet* CreateOrGetSpriteSheetFromFile( const std::string& filePath,
                                                 const IntVec2& size, bool flipV = false );
    Rasterizer* CreateOrGetRasterizer( CullMode cullMode, FillMode fillMode, WindOrder order );

    MaterialSheet* CreateOrGetMaterialSheetFromData( const std::string& name, const std::map<std::string, SpriteSheet*> data, const IntVec2& gridLayout );
    MaterialSheet* GetMaterialSheet( const std::string& name );


    Texture* AcquireMatchingRenderTarget( Texture* texture );
    void ReleaseRenderTarget( Texture* texture );
    size_t GetTotalTexturePoolCount() const;
    size_t GetFreeTexturePoolCount() const;

    void StartEffect( Texture* dst, Texture* src, ShaderProgram* shader );
    void StartEffect( Texture* dst, Texture* src, Material* material );
    void EndEffect();

    void BindMaterial( const Material* constMaterial );
    void BindShader( const Shader* shader );
    void BindShader( const std::string& fileName );
    void BindTexture( const Texture* constTexture );
    void BindNormalMap( const Texture* constTexture );

    void BindSampler( unsigned int index, const Sampler* sampler );
    void BindSampler( const std::string& fileName );

    void BindUniformBuffer( unsigned int slot, RenderBuffer* ubo ) const;

    void SetModelUBO( const Mat44& modelMatrix = Mat44::IDENTITY, const Rgba8& modelTint = Rgba8::WHITE, float specularFactor = .5f, float specularPower = 32.f );

    void SetAmbientLight(const Rgba8& color, float intensity);
    void EnableLight( size_t index, const Light& light );
    void DisableLight( size_t index );
    void SetFogData( const FogDataUBO& fogDataUBO );
    void DisableFog();
    bool CheckDepthState( DepthCompare compareFunction, bool writeDepthOnPass );

    void Draw( size_t numVertexes, size_t vertexOffset = 0 );
    void DrawIndexed( std::vector<VertexMaster>& vertexes, std::vector<unsigned int>& indexes );
    void DrawMesh( GPUMesh* mesh );
    void DrawVertexArray( size_t numVertexes, const Vertex_PCU* vertexes );
    void DrawVertexArray( const std::vector<VertexMaster>& vertexes );

    void CopyTexture( Texture* des, Texture* src ) const;

    void SaveShaderState( OUT_PARAM Shader* copyTo) const;
    void LoadShaderState( const Shader* copyFrom );
    void ReloadShaders();

protected:
    Sampler* CreateSamplerFromFile( const std::string& filePath );
    Shader* CreateShaderFromFile( const std::string& filePath );
    ShaderProgram* CreateShaderProgramFromFile( const std::string& filePath );
    BitmapFont* CreateBitmapFromFile( const std::string& filePath );
    SpriteSheet* CreateSpriteSheetFromFile( const std::string& filePath, const IntVec2& size, bool flipV = false );
    MaterialSheet* CreateMaterialSheetFromData( const std::map<std::string, SpriteSheet*> data, const IntVec2& gridLayout );

private:
    Camera* m_CurrentCamera = nullptr;
    Camera* m_EffectCamera = nullptr;
    SwapChain* m_SwapChain = nullptr;


    // Updated statefulness objects
    RenderBuffer* m_VertexBuffer = nullptr;
    IndexBuffer* m_IndexBuffer = nullptr;
    TextureBuffer* m_TextureBuffer = nullptr;
    TextureBuffer* m_NormalBuffer = nullptr;
    RenderBuffer* m_FrameUBO = nullptr;
    RenderBuffer* m_LightUBO = nullptr;
    RenderBuffer* m_CameraUBO = nullptr;
    RenderBuffer* m_ModelUBO = nullptr;
    RenderBuffer* m_FogUBO = nullptr;

    Shader* m_CurrentShader = nullptr;
    Shader* m_DefaultShader = nullptr;
    Shader* m_EffectShader = nullptr;
    bool m_ShaderIsDirty = true;

    std::vector<Texture*> m_TexturePool;
    int m_NumTexturesInPool = 0;            // Debug purposes

    Rasterizer* m_DefaultRasterizer = nullptr;
    const Rasterizer* m_CurrentRasterizer = nullptr;

    ID3D11BlendState* m_SolidBlendState;
    ID3D11BlendState* m_AlphaBlendState;
    ID3D11BlendState* m_AdditiveBlendState;

    ID3D11DepthStencilState* m_CurrentDepthStencil = nullptr;

    bool m_DuringDrawing = false;

    std::map<const std::string, Sampler*> m_LoadedSamplers;
    std::map<const std::string, Texture*> m_LoadedTextures;
    std::map<const std::string, Shader*> m_LoadedShaders;
    std::map<const std::string, ShaderProgram*> m_LoadedShaderPrograms;
    std::map<const std::string, BitmapFont*> m_LoadedBitmapFonts;
    std::map<const std::string, SpriteSheet*> m_LoadedSpriteSheets;
    std::map<const std::string, MaterialSheet*> m_LoadedMaterialSheets;
    std::vector<Rasterizer*> m_LoadedRasterizers;

    void CreateDefaultShaders();
    void CreateBuiltInTextures();
    void CreateBuiltInSamplers();
    void CreateBlendStates();

    BufferAttribute* m_CurrentLayout;
    void UpdateLayoutIfNeeded( BufferAttribute* newLayout);
    void UpdateLayout() const;

    void BindShader();
    void BindRasterizer( const Rasterizer* rasterizer );

    void SetRasterizer( CullMode cullMode, FillMode fillMode, WindOrder order );
    void SetBlendMode( BlendMode blendMode );
    void SetDepthTest( DepthCompare compareFunction, bool writeDepthOnPass );

    void DrawIfPending();
    void Finalize();
    void Flush() const;

    bool IsDrawPending() const;

    void BindVertexBuffer( RenderBuffer* vbo ) const;
    void BindIndexBuffer( IndexBuffer* ibo ) const;

    void ClearAndRebindState();
    void ReportLiveObjects();
    void CreateDebugModule();
    void DestroyDebugModule();
};
