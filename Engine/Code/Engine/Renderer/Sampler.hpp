#pragma once
#include "Engine/Core/Utils/XmlUtils.hpp"
#include <string>

//-----------------------------------------------------------------------------
// D3D11 Predefines
struct ID3D11SamplerState;
// Engine Predefines
class RenderContext;

enum class SamplerType
{
    POINT,
    BILINEAR,
    TRILINEAR,
};

enum class SamplerEdgeMode
{
    CLAMP,
    WRAP,
};

class Sampler
{
    friend class RenderContext;
public:
    ~Sampler();

    static Sampler* PopulateFromXmlElement( RenderContext* ctx, const XmlElement& element );

    ID3D11SamplerState* GetHandle() const { return m_Handle; }

private:
    RenderContext* m_Owner = nullptr;
    ID3D11SamplerState* m_Handle = nullptr;

    Sampler( RenderContext* ctx, SamplerType type, SamplerEdgeMode edgeMode );
};