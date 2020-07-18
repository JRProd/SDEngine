#include "Material.hpp"

Material::Material( RenderContext* ctx, Shader* shader )
    : m_Context( ctx )
      , m_Shader( shader )
{
    m_ModelData = new ConstantBuffer<ModelDataUBO>( ctx );
    m_ModelData->MakeClean();

    m_DiffuseTexture = new TextureBuffer( ctx );
    m_DiffuseTexture->MakeClean();
    m_NormalTexture = new TextureBuffer( ctx );
    m_NormalTexture->MakeClean();
}

Material::~Material()
{
    for( size_t bufferIndex = 0; bufferIndex < 8; ++bufferIndex )
    {
        if( m_ConstantBuffers[bufferIndex] != nullptr )
        {
            delete m_ConstantBuffers[ bufferIndex ];
            m_ConstantBuffers[ bufferIndex ] = nullptr;
        }

        if( m_TextureBuffers[bufferIndex] != nullptr )
        {
            delete m_TextureBuffers[ bufferIndex ];
            m_TextureBuffers[bufferIndex] = nullptr;
        }
    }

    if ( m_ModelData != nullptr )
    {
        delete m_ModelData;
        m_ModelData = nullptr;
    }

    if( m_DiffuseTexture != nullptr )
    {
        delete m_DiffuseTexture;
        m_DiffuseTexture = nullptr;
    }

    if( m_NormalTexture != nullptr )
    {
        delete m_NormalTexture;
        m_NormalTexture = nullptr;
    }

    if( m_DestroyShader && m_Shader != nullptr)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }

    for( size_t index = 0; index < 8; ++index )
    {
        if( m_DestroySamplers[index] && m_Samplers[index] != nullptr )
        {
            delete m_Samplers[ index ];
            m_Samplers[ index ] = nullptr;
        }
    }
}

STATIC Material* Material::CreateFromFile( RenderContext* ctx, const std::string& fileName )
{
    XmlDocument doc;
    doc.LoadFile( fileName.c_str() );
    return PopulateFromXmlElement( ctx, *doc.RootElement() );
}

STATIC Material* Material::PopulateFromXmlElement( RenderContext* ctx, const XmlElement& element )
{
    Material* createdMaterial = new Material( ctx, nullptr );
    const std::string programFile = ParseXmlAttribute( element, "program" );
    if( !programFile.empty() )
    {
        createdMaterial->m_Shader = ctx->CreateOrGetShaderFromFile( programFile );
    }
    else
    {
        createdMaterial->m_Shader = Shader::PopulateFromXmlElement( ctx, *element.FirstChildElement( "Shader" ) );
        createdMaterial->m_DestroyShader = true;
    }
    GUARANTEE_OR_DIE( createdMaterial->m_Shader != nullptr, "Material::PopulateFromXmlElement - Failed to create the shader" );

    const XmlElement* samplerElement = element.FirstChildElement( "Sampler" );
    while ( samplerElement != nullptr )
    {
        const unsigned int slot = ParseXmlAttribute( *samplerElement, "slot", -1 );
        Sampler* sampler = Sampler::PopulateFromXmlElement( ctx, *samplerElement );
        createdMaterial->UpdateSampler( slot, sampler );

        createdMaterial->m_DestroySamplers[ slot ] = true;
        samplerElement = samplerElement->NextSiblingElement( "Sampler" );
    }

    const XmlElement* textureElement = element.FirstChildElement( "Texture" );
    while ( textureElement != nullptr )
    {
        const unsigned int slot = ParseXmlAttribute( *textureElement, "slot", -1 );
        const std::string fileName = ParseXmlAttribute( *textureElement, "file" );
        Texture* texture = ctx->CreateOrGetTextureFromFile( fileName );
        createdMaterial->UpdateTexture( slot, texture );

        textureElement = textureElement->NextSiblingElement( "Texture" );
    }

    ModelDataUBO defaults = createdMaterial->m_ModelData->GetLocalBuffer();
    defaults.meshTint = ParseXmlAttribute( element, "tint", Rgba8::WHITE ).GetAsVec4();
    defaults.specularFactor = ParseXmlAttribute( element, "specularFactor", defaults.specularFactor );
    defaults.specularPower = ParseXmlAttribute( element, "specularPower", defaults.specularPower );
    createdMaterial->m_ModelData->UpdateLocalBuffer( defaults );

    return createdMaterial;
}

void Material::UpdateSampler( unsigned int index, Sampler* sampler )
{
    GUARANTEE_OR_DIE( index < 8, "Material::UpdateSampler - Index out of range" );

    m_Samplers[ index ] = sampler;
}

void Material::UpdateTexture( unsigned int index, Texture* texture )
{
    GUARANTEE_OR_DIE( index < 8, "Material::UpdateTexture - Index out of range" );

    if( m_TextureBuffers[index] == nullptr )
    {
        m_TextureBuffers[ index ] = new TextureBuffer( m_Context );
    }

    m_TextureBuffers[ index ]->UpdateLocalBuffer( texture );
}
