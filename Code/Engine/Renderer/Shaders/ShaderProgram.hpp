#pragma once

#include <string>

#include "Engine/Renderer/Buffers/BufferAttribute.hpp"

//-----------------------------------------------------------------------------
// D3D11 Predefines
struct ID3D10Blob;
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D11InputLayout;

// Engine Predefines
class RenderContext;
struct BuiltInShader;

enum ShaderType
{
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT,
};

class ShaderStage
{
public:
    union
    {
        ID3D11Resource* m_Handle = nullptr;
        ID3D11VertexShader* m_vs;
        ID3D11PixelShader* m_fs;
    };

    ShaderStage() = default;
    ~ShaderStage();

    bool Compile( RenderContext* ctx,
                  const std::string& fileName,
                  const void* source,
                  const size_t sourceByteLen,
                  ShaderType stage );

    bool Compile( RenderContext* ctx,
                  const char* shaderName,
                  const void* source,
                  const size_t sourceByteLen,
                  ShaderType stage );

    bool CompileBuiltIn( RenderContext* ctx,
                         const BuiltInShader& builtInShader,
                         ShaderType stage );

    const void* GetByteCode() const;
    size_t GetByteCodeLength() const;

    bool IsValid() const { return m_Handle != nullptr; }

private:
    ShaderType m_Type = SHADER_TYPE_FRAGMENT;

    ID3D10Blob* m_ByteCode = nullptr;

    bool CreateShaderFromByteCode( RenderContext* ctx, ID3D10Blob* byteCode, ShaderType stage );
};

class ShaderProgram
{
    friend class RenderContext;

public:
    ShaderProgram( RenderContext* context );
    ~ShaderProgram();

    bool CreateFromFile( const std::string& fileName );
    bool CreateFromBuiltIn( const BuiltInShader& builtInShader );
    ShaderStage* GetVertexShader() { return m_VertexStage; }
    ShaderStage* GetFragmentShader() { return m_FragmentStage; }

    ID3D11InputLayout* CreateOrGetInputLayout( const BufferAttribute* newLayout );

private:
    RenderContext* m_Owner = nullptr;
    std::string m_FileName;
    bool m_FromBuiltIn = false;

    ShaderStage* m_VertexStage = nullptr;
    ShaderStage* m_FragmentStage = nullptr;

    BufferAttribute* m_CurrentBufferLayout = nullptr;
    ID3D11InputLayout* m_InputLayout = nullptr;

    bool IsSameLayout( const BufferAttribute* layout );

    void Reload();
};