
#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4

struct FLightInfoBase
{
    float3 Color;
    float Intensity;
};

struct FAmbientLightInfo : FLightInfoBase
{
    float dummy;
};

struct FDirectionalLightInfo : FLightInfoBase
{
    float3 Direction;
};

struct FPointLightInfo : FLightInfoBase
{
    float3 Position;
    float AttenuationRadius;
    float LightFalloffExponent;
};

struct FSpotLightInfo : FPointLightInfo
{
    float3 Direction;
    float InnerConeAngle;
    float OuterConeAngle;
};

// 상위 쉐이더에서 모아서 통합 할 수 있을듯? 다른 객체들의 MVP와 함께.
cbuffer PerObject : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

cbuffer Lighting : register(b1)
{
    FAmbientLightInfo AmbientLight;
    FDirectionalLightInfo DirectionalLight;
    FPointLightInfo PointLights[NUM_POINT_LIGHT];
    FSpotLightInfo SpotLights[NUM_SPOT_LIGHT];
};

float4 CaculateAmbientLight(FAmbientLightInfo InLightInfo)
{
    float4 Result;

    return Result;
}

struct VS_IN
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUT
{
    float4 Position : SV_POSITION;
    float3 WorldNormal : NORMAL;
};
