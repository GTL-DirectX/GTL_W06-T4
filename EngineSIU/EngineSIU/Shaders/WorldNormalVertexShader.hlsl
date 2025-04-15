#include "ShaderCommon.hlsl"

struct VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    int materialIndex : MATERIAL_INDEX;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD2;
    int materialIndex : MATERIAL_INDEX;
    float3x3 tbn : TBN_MATRIX;
};

VS_OUT mainVS(VS_IN input)
{
    VS_OUT output;
    
    output.materialIndex = input.materialIndex;
    
    float4 worldPosition = mul(float4(input.position, 1), World);
    output.worldPos = worldPosition.xyz;
    output.position = mul(float4(input.position, 1.0), GetMVP());
  
    output.normal = normalize(mul(input.normal, (float3x3) MInverseTranspose));
    output.texcoord = input.texcoord;
    output.color = input.color;
    
    float3 biTangent = cross(input.normal, input.tangent);
    
    float3x3 tbn =
    {
        input.tangent.x, input.tangent.y, input.tangent.z,
        biTangent.x, biTangent.y, biTangent.z,
        input.normal.x, input.normal.y, input.normal.z 
    };
    output.tbn = tbn;
    
    return output;
}
