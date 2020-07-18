#include "Engine/Renderer/Shaders/ShaderProgram.hpp"

#include <d3dcompiler.h>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/IO/FileUtils.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shaders/BuiltInShaders.hpp"




static const char* GetDefaultEntryPointForStage( ShaderType type )
{
    switch ( type )
    {
        case SHADER_TYPE_VERTEX: return "vert_main";
        case SHADER_TYPE_FRAGMENT: return "frag_main";
        default: ERROR_AND_DIE( "ShaderProgram Type not implemented" );
    }
}

//-----------------------------------------------------------------------------
// ShaderProgram version to use
static const char* GetShaderModelForStage( ShaderType type )
{
    switch ( type )
    {
        case SHADER_TYPE_VERTEX: return "vs_5_0";
        case SHADER_TYPE_FRAGMENT: return "ps_5_0";
        default: ERROR_AND_DIE( "ShaderProgram Type not implemented" );
    }
}

ShaderStage::~ShaderStage()
{
    DX_SAFE_RELEASE( m_ByteCode );
    DX_SAFE_RELEASE( m_Handle );
    DX_SAFE_RELEASE( m_vs );
    DX_SAFE_RELEASE( m_fs );
}

//-----------------------------------------------------------------------------
// HLSL - High Level Shading Language
// Compile: HLSL -> Byte code
// Link Byte code: Device Assembly (what we need to get to) - This is device specific
bool ShaderStage::Compile( RenderContext* ctx,
                           const std::string& fileName,
                           const void* source,
                           const size_t sourceByteLen,
                           ShaderType stage )
{
    return Compile( ctx, fileName.c_str(), source, sourceByteLen, stage );
}

bool ShaderStage::Compile( RenderContext* ctx, 
                           const char* shaderName, 
                           const void* source, 
                           const size_t sourceByteLen, 
                           ShaderType stage )
{
    const char* entrypoint = GetDefaultEntryPointForStage( stage );
    const char* shaderModel = GetShaderModelForStage( stage );

    DWORD compileFlags = 0;
#if defined(DEBUG_SHADERS)
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
    compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
    compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

    ID3DBlob* byteCode = nullptr;
    ID3DBlob* errors = nullptr;

    HRESULT hr = ::D3DCompile( source,                                      // Plain text HLSL code
                               sourceByteLen,                               // byte length of plain text source code
                               shaderName,                                  // optional, used for error messages
                               nullptr,                             // Pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
                               D3D_COMPILE_STANDARD_FILE_INCLUDE,           // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
                               entrypoint,                                  // Entry point to the shader
                               shaderModel,                          // Compile target (MSDN - "Specifying compiler targets"
                               compileFlags,                          // Flags that control compilation
                               0,                                     // Effect Flags (we will not be doing Effect Files)
                               OUT & byteCode,                              // [OUT] ID3DBlob (buffer) that will store the byte code
                               OUT & errors );                    // [OUT] ID3DBlob (buffer) that will store error information;

    if ( FAILED( hr ) )
    {

        if ( errors != nullptr )
        {
            std::string shaderType;
            switch ( stage )
            {
                case SHADER_TYPE_VERTEX:
                    shaderType = "Vertex";
                    break;
                case SHADER_TYPE_FRAGMENT:
                    shaderType = "Fragment";
                    break;
                default:
                    shaderType = "UNKNOWN";
                    break;
            }
            char* errorString = (char*) errors->GetBufferPointer();
            DebuggerPrintf( "Failed to compile [%s-%s]. Compiler gave the following errors;\n%s",
                            shaderName,
                            shaderType.c_str(),
                            errorString );
        }

        if ( shaderName != BuiltInShader::ERROR_SHADER.builtInName )
        {
            DebuggerPrintf( "Fall back to using the built-in error shader\n" );
            return CompileBuiltIn( ctx, BuiltInShader::ERROR_SHADER, stage );
        }
        else
        {
            DebugBreak();
        }
    }

    DX_SAFE_RELEASE( errors );

    return CreateShaderFromByteCode( ctx, byteCode, stage );
}

bool ShaderStage::CompileBuiltIn( RenderContext* ctx, const BuiltInShader& builtInShader, ShaderType stage )
{
    const size_t sourceByteLen = strlen( (const char*)builtInShader.sourceCode );
    return Compile( ctx, builtInShader.builtInName, builtInShader.sourceCode, sourceByteLen, stage );
}

const void* ShaderStage::GetByteCode() const
{
    return m_ByteCode->GetBufferPointer();
}

size_t ShaderStage::GetByteCodeLength() const
{
    return m_ByteCode->GetBufferSize();
}

bool ShaderStage::CreateShaderFromByteCode( RenderContext* ctx, ID3D10Blob* byteCode, ShaderType stage )
{
    ID3D11Device* device = ctx->m_Device;
    const void* byteCodePtr = byteCode->GetBufferPointer();
    size_t byteCodeSize = byteCode->GetBufferSize();

    HRESULT hr;

    switch ( stage )
    {
        case SHADER_TYPE_VERTEX:
            hr = device->CreateVertexShader( byteCodePtr, byteCodeSize, nullptr, &m_vs );
            GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to link shader stage" );
            break;
        case SHADER_TYPE_FRAGMENT:
            hr = device->CreatePixelShader( byteCodePtr, byteCodeSize, nullptr, &m_fs );
            GUARANTEE_OR_DIE( SUCCEEDED( hr ), "Failed to link shader stage" );
            break;
        default: ERROR_AND_DIE( "ShaderProgram Type not implemented" );
    }


    // TODO: Figure out what to do with byte code;
    // byte code
    m_Type = stage;
    if ( stage == SHADER_TYPE_VERTEX )
    {
        m_ByteCode = byteCode;
    }
    else
    {
        DX_SAFE_RELEASE( byteCode );
        m_ByteCode = nullptr;
    }

    return IsValid();
}

ShaderProgram::ShaderProgram( RenderContext* context )
    : m_Owner( context )
{
}

ShaderProgram::~ShaderProgram()
{
    DX_SAFE_RELEASE( m_InputLayout );

    delete m_VertexStage;
    delete m_FragmentStage;
}

bool ShaderProgram::CreateFromFile( const std::string& fileName )
{
    m_FileName = fileName;

    size_t fileSize = 0;
    void* src = FileReadToNewBuffer( fileName, &fileSize );
    GUARANTEE_OR_DIE( src != nullptr, Stringf( "ShaderProgram::CreateFromFile - Failed to load file %s", fileName.c_str() ) );

    m_VertexStage = new ShaderStage();
    m_VertexStage->Compile( m_Owner, fileName, src, fileSize, SHADER_TYPE_VERTEX );

    m_FragmentStage = new ShaderStage();
    m_FragmentStage->Compile( m_Owner, fileName, src, fileSize, SHADER_TYPE_FRAGMENT );

    delete[] src;
    fileSize = 0;

    return m_VertexStage->IsValid() && m_FragmentStage->IsValid();
}

bool ShaderProgram::CreateFromBuiltIn( const BuiltInShader& builtInShader )
{
    m_FromBuiltIn = true;

    m_VertexStage = new ShaderStage();
    m_VertexStage->CompileBuiltIn( m_Owner, builtInShader, SHADER_TYPE_VERTEX );

    m_FragmentStage = new ShaderStage();
    m_FragmentStage->CompileBuiltIn( m_Owner, builtInShader, SHADER_TYPE_FRAGMENT );

    return m_VertexStage->IsValid() && m_FragmentStage->IsValid();
}

ID3D11InputLayout* ShaderProgram::CreateOrGetInputLayout( const BufferAttribute* newLayout )
{
    if( newLayout == nullptr )
    {
        newLayout = Vertex_PCU::LAYOUT;
    }

    if ( IsSameLayout( newLayout ) )
    {
        return m_InputLayout;
    }

    if ( m_InputLayout != nullptr )
    {
        DX_SAFE_RELEASE( m_InputLayout );
    }

    ID3D11Device* device = m_Owner->m_Device;

    unsigned char sizeOfBufferLayout = 0;
    std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDescription;
    const BufferAttribute* currentAttribute = &newLayout[ 0 ];
    while ( currentAttribute->type != BufferFormatType::FINAL )
    {
        D3D11_INPUT_ELEMENT_DESC attributeDesc;
        attributeDesc.SemanticName = currentAttribute->name.c_str();
        attributeDesc.SemanticIndex = 0;                                        // Array Element - currently don't use
        attributeDesc.Format = BufferFormatTypeToDXType( currentAttribute->type ); // Type of data
        attributeDesc.InputSlot = 0;                                            // Buffer index slots
        attributeDesc.AlignedByteOffset = currentAttribute->offset;
        attributeDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        attributeDesc.InstanceDataStepRate = 0;

        vertexDescription.push_back( attributeDesc );
        sizeOfBufferLayout++;
        currentAttribute = &newLayout[ sizeOfBufferLayout ];
    }

    // Create th input layout
    device->CreateInputLayout( vertexDescription.data(),
                               sizeOfBufferLayout,
                               m_VertexStage->GetByteCode(),
                               m_VertexStage->GetByteCodeLength(),
                               &m_InputLayout );

    m_CurrentBufferLayout = const_cast<BufferAttribute*>(newLayout);
    return m_InputLayout;
}

bool ShaderProgram::IsSameLayout( const BufferAttribute* layout )
{
    if ( m_CurrentBufferLayout == nullptr ) { return false; }
    if ( m_CurrentBufferLayout == layout ) { return true; }

    int currentTest = 0;
    const BufferAttribute* shaderValue = &m_CurrentBufferLayout[ currentTest ];
    const BufferAttribute* otherValue = &layout[ currentTest ];

    while ( *shaderValue == *otherValue )
    {
        if ( shaderValue->type == BufferFormatType::FINAL )
        {
            return true;
        }

        currentTest++;
        shaderValue = &m_CurrentBufferLayout[ currentTest ];
        otherValue = &layout[ currentTest ];
    }

    return false;
}

void ShaderProgram::Reload()
{
    if ( m_FromBuiltIn ) { return; }
    
    delete m_VertexStage;
    delete m_FragmentStage;

    size_t fileSize = 0;
    void* src = FileReadToNewBuffer( m_FileName, &fileSize );

    m_VertexStage = new ShaderStage();
    m_FragmentStage = new ShaderStage();

    m_VertexStage->Compile( m_Owner, m_FileName, src, fileSize, SHADER_TYPE_VERTEX );

    m_FragmentStage->Compile( m_Owner, m_FileName, src, fileSize, SHADER_TYPE_FRAGMENT );
}



