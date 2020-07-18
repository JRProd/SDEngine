#include "Rasterizer.hpp"

#include "Engine/Renderer/RenderContext.hpp"

static D3D11_CULL_MODE ToD3D11Type(const CullMode cullMode)
{
    switch (cullMode)
    {
    case CullMode::NONE: return D3D11_CULL_NONE;
    case CullMode::FRONT: return D3D11_CULL_FRONT;
    case CullMode::BACK: return D3D11_CULL_BACK;
    default: ERROR_AND_DIE("Unsupported CullMode");
    }
}

static D3D11_FILL_MODE ToD3D11Type(const FillMode fillMode)
{
    switch (fillMode)
    {
    case FillMode::TRIANGLE: return D3D11_FILL_SOLID;
    case FillMode::LINE: return D3D11_FILL_WIREFRAME;
    default: ERROR_AND_DIE("Unsupported FillMode");
    }
}

static BOOL ToD3D11Type(const WindOrder windOrder)
{
    switch (windOrder)
    {
    case WindOrder::CLOCKWISE: return FALSE;
    case WindOrder::COUNTER_CLOCKWISE: return TRUE;
    default: ERROR_AND_DIE("Unsupported WindOrder");
    }
}

Rasterizer::Rasterizer(RenderContext* ctx)
    : m_Owner(ctx)
      , m_CullMode(CullMode::BACK)
      , m_FillMode(FillMode::TRIANGLE)
      , m_WindOrder(WindOrder::COUNTER_CLOCKWISE)
{
    CreateRasterState();
}

Rasterizer::Rasterizer(RenderContext* ctx, CullMode cull, FillMode fill, WindOrder order)
    : m_Owner(ctx)
      , m_CullMode(cull)
      , m_FillMode(fill)
      , m_WindOrder(order)
{
    CreateRasterState();
}

Rasterizer::~Rasterizer()
{
    DX_SAFE_RELEASE(m_RasterState);
}

void Rasterizer::CreateRasterState()
{
    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.CullMode = ToD3D11Type(m_CullMode);
    desc.FillMode = ToD3D11Type(m_FillMode);
    desc.FrontCounterClockwise = ToD3D11Type(m_WindOrder);
    desc.SlopeScaledDepthBias = TRUE;

    ID3D11Device* device = m_Owner->m_Device;
    device->CreateRasterizerState(&desc, &m_RasterState);
}

bool Rasterizer::operator==(const Rasterizer& rhs) const
{
    return m_CullMode == rhs.m_CullMode && m_FillMode == rhs.m_FillMode && m_WindOrder == rhs.
        m_WindOrder;
}

bool Rasterizer::Equals(const CullMode cullMode, const FillMode fillMode, const WindOrder order) const
{
    return m_CullMode == cullMode && m_FillMode == fillMode && m_WindOrder == order;
}

bool Rasterizer::operator!=(const Rasterizer& rhs) const
{
    return m_CullMode != rhs.m_CullMode || m_FillMode != rhs.m_FillMode || m_WindOrder != rhs.
        m_WindOrder;
}
