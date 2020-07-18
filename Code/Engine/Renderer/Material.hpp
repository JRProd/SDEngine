#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Buffers/ConstantBuffer.hpp"

class Texture;
class RenderContext;
class TextureBuffer;
class ShaderProgram;
class RenderBuffer;

enum BufferSlot: unsigned int
{
    UBO_FRAME_SLOT = 0,
    UBO_CAMERA_SLOT = 1,
    UBO_OBJECT_SLOT = 2,
    UBO_LIGHT_SLOT = 3,
    UBO_FOG_SLOT = 4,
};

class Material
{
    friend class RenderContext;

public:
    ConstantBuffer<ModelDataUBO>* m_ModelData = nullptr;
    TextureBuffer* m_DiffuseTexture = nullptr;
    TextureBuffer* m_NormalTexture = nullptr;

    Material( RenderContext* ctx, Shader* shader );
    ~Material();

    static Material* CreateFromFile( RenderContext* ctx, const std::string& fileName );

    template<class BufferStruct>
    void UpdateConstantBuffer( unsigned int index, BufferStruct buffer );

    void UpdateSampler( unsigned int index, Sampler* sampler );
    void UpdateTexture( unsigned int index, Texture* texture );

private:
    RenderContext* m_Context = nullptr;
    Shader* m_Shader = nullptr;
    bool m_DestroyShader = false;

    Sampler* m_Samplers[ 8 ] = { nullptr };
    bool m_DestroySamplers[ 8 ] = { false };
    RenderBuffer* m_ConstantBuffers[ 8 ] = {nullptr};
    TextureBuffer* m_TextureBuffers[ 8 ] = {nullptr};

    static Material* PopulateFromXmlElement( RenderContext* ctx, const XmlElement& element );
};

template<class BufferStruct>
void Material::UpdateConstantBuffer( unsigned int index, BufferStruct buffer )
{
    GUARANTEE_OR_DIE( index < 8, "Material::UpdateConstantBuffer - Index out of range" );

    if ( m_ConstantBuffers[ index ] == nullptr )
    {
        m_ConstantBuffers[ index ] = new ConstantBuffer<BufferStruct>( m_Context );
    }

    ConstantBuffer<BufferStruct>* constantBuffer = ConstantBuffer<BufferStruct>::FromRenderBuffer(
        m_ConstantBuffers[ index ] );
    constantBuffer->UpdateLocalBuffer( buffer );
}
