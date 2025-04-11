
#define NUM_POINT_LIGHT 4
#define NUM_SPOT_LIGHT 4

struct FAmbientLightInfo
{
    float3 Color;
    float Intensity;
};

struct FDirectionalLightInfo
{
    float3 Color;
    float Intensity;
    float3 Direction;
    float Pad;
};

struct FPointLightInfo
{
    float3 Color;
    float Intensity;
    float3 Position;
    float AttenuationRadius;
    float LightFalloffExponent;
    float2 Pad;
};

struct FSpotLightInfo
{
    float3 Color;
    float Intensity;
    float3 Position;
    float AttenuationRadius;
    float LightFalloffExponent;
    float3 Direction;
    float InnerConeAngle;
    float OuterConeAngle;
    float2 Pad;
};

// 상위 쉐이더에서 모아서 통합 할 수 있을듯? 다른 객체들의 MVP와 함께.
cbuffer PerObject : register(b0)
{
    row_major matrix World;
    row_major matrix View;
    row_major matrix Projection;
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

VS_OUT Uber_VS(VS_IN Input)
{
    VS_OUT output;
#if LIGHTING_MODEL_GOURAUD

#elif LIGHTING_MODEL_LAMBERT

#elif LIGHTING_MODEL_PHONG

#endif
    
    return output;
}

float4 Uber_PS(VS_OUT Input) : SV_TARGET
{
    //float4 finalPixel = TextureColor;
    //finalPixel += Emissive;
#if LIGHTING_MODEL_GOURAUD
#elif LIGHTING_MODEL_LAMBERT
    finalPixel += CalculateAmbientLight(...);
    for(It : PointLights)
    {
        finalPixel += CalculatePointLight(...);
    }
#elif LIGHTING_MODEL_PHONG
    // Specular Reflectance
#endif
    return float4(0.33, 0.33, 0.33, 0.33);
    //return finalPixel;
}
