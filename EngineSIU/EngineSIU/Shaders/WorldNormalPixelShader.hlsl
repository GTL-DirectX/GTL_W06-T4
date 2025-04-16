#include "ShaderCommon.hlsl"

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPos : WORLD_POS;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3x3 tbn : TBN_MATRIX;
};

struct PS_OUT
{
    float4 color : SV_Target0;
};

Texture2D BumpTexture : register(t3);
SamplerState Sampler : register(s0);

PS_OUT mainPS(VS_OUT input) : SV_TARGET
{
    PS_OUT output;
    
    float3 normal = input.normal;
    
    if (Material.TextureSlotMask & (1 << 3))
    {
        float3x3 MInverse3x3 = float3x3(
            MInverseTranspose._11, MInverseTranspose._12, MInverseTranspose._13,
            MInverseTranspose._21, MInverseTranspose._22, MInverseTranspose._23,
            MInverseTranspose._31, MInverseTranspose._32, MInverseTranspose._33
        );
        
        float3 normalMap = BumpTexture.Sample(Sampler, input.texcoord).rgb;
        normalMap = normalMap * 2.0f - 1.0f;
        normal = normalize(mul(mul(normalMap, input.tbn), MInverse3x3));
    }
    
    float3 normalColor = (normal + 1.0f) * 0.5f;
    output.color = float4(normalColor, 1.0f);
    
    return output;
}
