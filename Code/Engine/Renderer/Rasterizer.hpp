#pragma once
#include "D3D11Common.hpp"

#include "Engine/Renderer/Shaders/Shader.hpp"

class RenderContext;

class Rasterizer
{
public:
    explicit Rasterizer( RenderContext* ctx );
    Rasterizer( RenderContext* ctx, CullMode cull, FillMode fill, WindOrder order );
    ~Rasterizer();

    CullMode GetCullMode() const { return m_CullMode; }
    FillMode GetFillMode() const { return m_FillMode; }
    WindOrder GetWindOrder() const { return m_WindOrder; }

    ID3D11RasterizerState* GetRasterState() const { return m_RasterState; }

    bool operator==( const Rasterizer& rhs ) const;
    bool Equals( CullMode cullMode, FillMode fillMode, WindOrder order ) const;
    bool operator!=( const Rasterizer& rhs ) const;

private:
    void CreateRasterState();

    RenderContext* m_Owner;
    ID3D11RasterizerState* m_RasterState = nullptr;

    CullMode m_CullMode;
    FillMode m_FillMode;
    WindOrder m_WindOrder;
};
