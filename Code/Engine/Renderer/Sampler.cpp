#include "Sampler.hpp"

#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

static D3D11_FILTER GetD3D11From( const SamplerType samplerType )
{
    if ( samplerType == SamplerType::POINT ) { return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR; }
    if ( samplerType == SamplerType::BILINEAR ) { return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT; }

    return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
}

static D3D11_TEXTURE_ADDRESS_MODE GetD3D11From( const SamplerEdgeMode samplerEdgeMode )
{
    if ( samplerEdgeMode == SamplerEdgeMode::CLAMP ) { return D3D11_TEXTURE_ADDRESS_CLAMP; }
    if ( samplerEdgeMode == SamplerEdgeMode::WRAP ) { return D3D11_TEXTURE_ADDRESS_WRAP; }

    return D3D11_TEXTURE_ADDRESS_CLAMP;
}

static SamplerType GetSampleTypeFromXml( const XmlElement& element, const SamplerType defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "type" );
    if ( string == "point" ) { return SamplerType::POINT; }
    if ( string == "linear" ) { return SamplerType::BILINEAR; }
    if ( string == "trilinear" ) { return SamplerType::TRILINEAR; }

    return defaultValue;
}

static SamplerEdgeMode GetSamplerEdgeModeFromXml( const XmlElement& element, const SamplerEdgeMode defaultValue )
{
    const std::string string = ParseXmlAttribute( element, "edgeMode" );
    if ( string == "wrap" ) { return SamplerEdgeMode::WRAP; }
    if ( string == "clamp" ) { return SamplerEdgeMode::CLAMP; }

    return defaultValue;
}

Sampler::Sampler( RenderContext* ctx, const SamplerType type, const SamplerEdgeMode edgeMode )
    : m_Owner( ctx )
{
    ID3D11Device* device = m_Owner->m_Device;

    D3D11_SAMPLER_DESC desc;
    desc.Filter = GetD3D11From( type );
    desc.AddressU = GetD3D11From( edgeMode );
    desc.AddressV = desc.AddressU;
    desc.AddressW = desc.AddressU;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 0;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.BorderColor[ 0 ] = 0.f;
    desc.BorderColor[ 1 ] = 0.f;
    desc.BorderColor[ 2 ] = 0.f;
    desc.BorderColor[ 3 ] = 0.f;
    desc.MinLOD = 0.f;
    desc.MaxLOD = 0.f;

    device->CreateSamplerState( &desc, &m_Handle );
}

Sampler::~Sampler()
{
    DX_SAFE_RELEASE( m_Handle );
}

Sampler* Sampler::PopulateFromXmlElement( RenderContext* ctx, const XmlElement& element )
{
    const SamplerType type = GetSampleTypeFromXml( element, SamplerType::POINT );
    const SamplerEdgeMode edgeMode = GetSamplerEdgeModeFromXml( element, SamplerEdgeMode::CLAMP );
    return new Sampler(ctx, type, edgeMode);
}
