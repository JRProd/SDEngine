#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Buffers/BufferAttribute.hpp"

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif

#define RENDER_DEBUG
// #define RENDER_DEBUG_VERBOSE
#define DX_SAFE_RELEASE( ptr ) if ( ptr != nullptr ) { ptr->Release(); ptr = nullptr; }

//-----------------------------------------------------------------------------
// Adding DX11
#define INITGUID
#include <d3d11.h>      // d3d11 specific objects
#include <dxgi.h>       // shared library used across multiple dx graphical interfaces
#include <dxgidebug.h>  // debug utility

#pragma comment( lib, "d3d11.lib")          // needed a01
#pragma comment( lib, "dxgi.lib")           // needed a01
#pragma comment( lib, "d3dcompiler.lib")    // needed when we get to shaders

DXGI_FORMAT BufferFormatTypeToDXType( BufferFormatType type );
