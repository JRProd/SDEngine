#include "D3D11Common.hpp"
#pragma once

DXGI_FORMAT BufferFormatTypeToDXType( BufferFormatType type )
{
    switch ( type )
    {
        case BufferFormatType::VEC2: return DXGI_FORMAT_R32G32_FLOAT;
        case BufferFormatType::VEC3: return DXGI_FORMAT_R32G32B32_FLOAT;
        case BufferFormatType::R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        default:
            ERROR_AND_DIE( "BufferFormatType not supported for DX Type" );
    }
}
