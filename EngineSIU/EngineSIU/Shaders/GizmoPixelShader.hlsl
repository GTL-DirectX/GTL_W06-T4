// GizmoPixelShader.hlsl

#include "ShaderCommon.hlsl"

cbuffer FlagConstants : register(b0)
{
    bool IsLit;
    float3 flagPad0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD2;
    int materialIndex : MATERIAL_INDEX;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    // float4 UUID : SV_Target1; // [비활성화 상태]
};

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;

    float3 baseColor = Material.DiffuseColor;

    output.color = float4(baseColor, 1.0f);

    // if (isSelected)
    // {
    //     output.color += float4(0.5f, 0.5f, 0.5f, 1.0f); // 선택 시 강조
    // }

    return output.color;
}
