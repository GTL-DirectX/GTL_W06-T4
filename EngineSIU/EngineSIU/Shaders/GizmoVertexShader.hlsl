// GizmoVertexShader.hlsl

#include "ShaderCommon.hlsl"

struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    int materialIndex : MATERIAL_INDEX;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD1;
    int materialIndex : MATERIAL_INDEX;
};

VS_OUT mainVS(VS_INPUT input)
{
    VS_OUT output;

    output.materialIndex = input.materialIndex;

    float4 worldPosition = mul(input.position, World);
    output.worldPos = worldPosition.xyz;

    float4 viewPosition = mul(worldPosition, View);
    output.position = mul(viewPosition, Projection);

    output.color = input.color;

    // 노멀을 월드 공간으로 변환
    output.normal = normalize(mul(input.normal, (float3x3) MInverseTranspose));

    output.texcoord = input.texcoord;

    return output;
}
