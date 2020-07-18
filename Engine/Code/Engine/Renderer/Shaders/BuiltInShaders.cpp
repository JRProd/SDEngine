#include "BuiltInShaders.hpp"

BuiltInShader::BuiltInShader( const char* name, const char* source )
    : builtInName( name )
    , sourceCode( source )
{
}

BuiltInShader BuiltInShader::ERROR_SHADER = BuiltInShader( "ErrorShader", R"(
struct vs_input_t
{
   // we are not defining our own input data; 
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer time_constants : register(b0)
{
    float SYSTEM_TIME_SECONDS;
    float SYSTEM_TIME_DELTA_SECONDS;
};

cbuffer camera_constants : register(b1)
{
    float4x4 CAMERA_TO_CLIP_TRANSFORM;
    float4x4 WORLD_TO_CAMERA;

    float3 CAMERA_POSITION;
};

cbuffer object_constants : register(b2)
{
    float4x4 OBJECT_MODEL;
    float4 OBJECT_TINT;

    float SPECULAR_FACTOR;
    float SPECULAR_POWER;
};

struct light_t
{
    float3 worldPosition;

    float4 color;

    float3 attenuation;
};

cbuffer light_constants: register(b3)
{
    float4 AMBIENT;
    light_t LIGHT;
};

Texture2D <float4> tDiffuse : register(t0);
Texture2D <float4> tNormal : register(t1);

SamplerState sSampler : register(s0);

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
v2f_t vert_main( vs_input_t input )
{
    v2f_t v2f = (v2f_t) 0;

    // forward vertex input onto the next stage
    float4 worldPos = float4(input.position, 1.0f);

    float4 objectPos = mul( OBJECT_MODEL, worldPos );
    float4 cameraPos = mul( WORLD_TO_CAMERA, objectPos );
    float4 clipPos = mul( CAMERA_TO_CLIP_TRANSFORM, cameraPos );

    v2f.position = clipPos;
    v2f.color = input.color * OBJECT_TINT;
    v2f.uv = input.uv;

    return v2f;
}

//--------------------------------------------------------------------------------------
// Fragment Shader
// 
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
float4 frag_main( v2f_t input ): SV_Target0
{
    float4 color = tDiffuse.Sample( sSampler, input.uv );

    if( input.uv.x < .5 && input.uv.y < .5 )
    {
        color = float4(1.f, 0.f, 1.f, 1.f);
    }
    else if ( input.uv.x >= .5 && input.uv.y < .5)
    {
        color = float4(1.f, 1.f, 0.f, 1.f);
    }
    else if ( input.uv.x < .5 && input.uv.y >= .5)
    {
        color = float4(1.f, 1.f, 0.f, 1.f);
    }
    else 
    {
        color = float4(1.f, 0.f, 1.f, 1.f);
    }

    return color;
}

)" );
