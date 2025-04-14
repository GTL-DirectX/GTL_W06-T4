#ifndef __COMMON_SHADER_BUFFERS__
#define __COMMON_SHADER_BUFFERS__

#define MERGE(a, b) a##b
#define REG(slot) register(MERGE(b, slot))

#define CB_SLOT_PerObject    10
#define CB_SLOT_Camera       11
#define CB_SLOT_Material     12
cbuffer PerObject : REG(CB_SLOT_PerObject)
{
    row_major matrix World;
    row_major matrix View;
    row_major matrix Projection;
    row_major matrix MInverseTranspose;
};

cbuffer CameraConstants : REG(CB_SLOT_Camera)
{
    float3 CameraPosition;
    float CameraConstantsPad;
};

struct FMaterial
{
    float3 DiffuseColor;
    float TransparencyScalar;
    float3 AmbientColor;//원래 4?
    float DensityScalar;
    float3 SpecularColor;
    float SpecularScalar;
    float3 EmissiveColor;
    float MaterialPad0;
    uint TextureSlotMask;
};
cbuffer MaterialConstants : REG(CB_SLOT_Material)
{
    FMaterial Material;
};
float4x4 GetMVP()
{
    return mul(World, mul(View, Projection));
}
#endif // __COMMON_SHADER_BUFFERS__
