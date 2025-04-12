#include "ShaderCommon.hlsl"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer SubUVConstants : register(b0)
{
    float2 uvOffset;
    float2 uvScale;
};

cbuffer UUIDConstant : register(b1)
{
    float4 UUID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSOutput
{
    float4 color : SV_Target0;
    float4 uuid : SV_Target1;
};

float4 mainPS(PSInput input) : SV_TARGET
{
    PSOutput output;

    float2 uv = input.texCoord * uvScale + uvOffset;
    float4 sampled = gTexture.Sample(gSampler, uv);

    const float threshold = 0.1f;

    if (sampled.r < threshold && sampled.g < threshold && sampled.b < threshold || sampled.a < threshold)
    {
        discard;
    }

    output.color = sampled;
    output.uuid = UUID;

    return output.color;
}
