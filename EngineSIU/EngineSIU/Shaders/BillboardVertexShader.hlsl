// BillboardVertexShader.hlsl

#include "ShaderCommon.hlsl"

struct VS_IN
{
    float3 Position : POSITION;
    float2 Texcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 Position : SV_POSITION;
    float2 Texcoord : TEXCOORD;
};

VS_OUT mainVS(VS_IN Input)
{
    VS_OUT Output;

    float4 worldPos = mul(float4(Input.Position, 1.0f), World);
    float4 viewPos = mul(worldPos, View);
    Output.Position = mul(viewPos, Projection);

    Output.Texcoord = Input.Texcoord;

    return Output;
}
