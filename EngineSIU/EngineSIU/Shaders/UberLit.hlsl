#include "ShaderCommon.hlsl"

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
    float3 Pad;
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

struct LightFactors
{
    float3 Diffuse;
    float3 Specular;
};

Texture2D DiffuseTexture : register(t0);
Texture2D AmbientTexture : register(t1);
Texture2D SpecularTexture : register(t2);
Texture2D BumpTexture : register(t3);
Texture2D AlphaTexture : register(t4);
StructuredBuffer<FPointLightInfo> PointLights : register(t5);
StructuredBuffer<FSpotLightInfo> SpotLights : register(t6);
SamplerState Sampler : register(s0);
cbuffer Lighting : register(b0)
{
    FAmbientLightInfo AmbientLight;
    FDirectionalLightInfo DirectionalLight;
};
cbuffer FlagConstants : register(b1)
{
    bool IsLit;
    float3 flagPad0;
}

cbuffer SubMeshConstants : register(b2)
{
    bool IsSelectedSubMesh;
    float3 SubMeshPad0;
}

cbuffer TextureConstants : register(b3)
{
    float2 UVOffset;
    float2 TexturePad0;
}

struct VS_IN
{
    float3 position : POSITION; // 버텍스 위치
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR; // 버텍스 색상
};

struct VS_OUT
{
    float4 position : SV_POSITION; // 클립 공간으로 변환된 화면 좌표
    float3 worldPos : WORLD_POS; // 월드 공간 위치 (조명용)
    float4 color : COLOR; // 버텍스 컬러 또는 머티리얼 베이스 컬러
    float3 normal : NORMAL; // 월드 공간 노멀
    float2 texcoord : TEXCOORD; // UV 좌표
    float3x3 tbn : TBN_MATRIX; // TBN Matrix for normal mapping
};

struct PS_OUT
{
    float4 color : SV_Target0;
};

// 모든 라이트 계산에서 사용할 공통 함수
float ComputeDistanceAttenuation(float distance, float radius, float exponent)
{
    return pow(saturate(1.0f - distance / radius), exponent);
}

float ComputeSpotlightFactor(float3 lightDir, float3 spotDirection, float innerAngle, float outerAngle)
{
    float cosTheta = dot(lightDir, normalize(-spotDirection));
    return smoothstep(cos(outerAngle), cos(innerAngle), cosTheta);
}

LightFactors ComputeBlinnPhong(float3 lightColor, float intensity, float3 lightDir, float3 viewDir, float3 normal, float attenuation)
{
    float3 halfDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0f);

    float specAngle = saturate(dot(normal, halfDir)); // 안전하게 클램핑
    float spec = pow(specAngle, Material.SpecularScalar);

    // float3 diffuse = lightColor * intensity * Material.DiffuseColor * diff * attenuation;
    float3 diffuse = lightColor * intensity * diff * attenuation;
    float3 specular = lightColor * intensity * spec * attenuation;

    LightFactors factor;
    factor.Diffuse = saturate(diffuse);
    factor.Specular = saturate(specular);
    //factor.Specular = float3(0.0f, 0.0f, 0.0f);
    return factor;
}

float4 CalculateGouraudLighting(float3 worldPosition, float3 worldNormal)
{
    float3 totalLight = AmbientLight.Color * AmbientLight.Intensity;
    
    // Directional Light
    float3 dirLightDir = normalize(-DirectionalLight.Direction);
    float dirDiffuse = saturate(dot(worldNormal, dirLightDir));
    totalLight += DirectionalLight.Color * DirectionalLight.Intensity * dirDiffuse;

    // Point Lights
    uint lightCount;
    uint lightStride;
    PointLights.GetDimensions(lightCount, lightStride);
    for (int i = 0; i < lightCount; i++)
    {
        float3 toLight = PointLights[i].Position - worldPosition;
        float distance = length(toLight);
        if (distance > PointLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float attenuation = ComputeDistanceAttenuation(distance,
            PointLights[i].AttenuationRadius,
            PointLights[i].LightFalloffExponent);
        
        float diffuse = saturate(dot(worldNormal, lightDir));
        totalLight += PointLights[i].Color * PointLights[i].Intensity 
            * diffuse * attenuation;
    }

    // Spot Lights
    SpotLights.GetDimensions(lightCount,lightStride);
    for (int i = 0; i < lightCount; ++i)
    {
        float3 toLight = SpotLights[i].Position - worldPosition;
        float distance = length(toLight);
        if (distance > SpotLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float attenuation = ComputeDistanceAttenuation(distance,
            SpotLights[i].AttenuationRadius,
            SpotLights[i].LightFalloffExponent);
        
        float spotFactor = ComputeSpotlightFactor(lightDir,
            SpotLights[i].Direction,
            SpotLights[i].InnerConeAngle,
            SpotLights[i].OuterConeAngle);

        float diffuse = saturate(dot(worldNormal, lightDir));
        totalLight += SpotLights[i].Color * SpotLights[i].Intensity 
            * diffuse * attenuation * spotFactor;
    }

    return float4(totalLight, 1.0f);
}

float3 CalculateLambertLighting(float3 worldPosition, float3 worldNormal)
{
    float3 ambientSum = AmbientLight.Color * AmbientLight.Intensity * Material.AmbientColor;
    float3 diffuseSum = float3(0.0f, 0.0f, 0.0f);
    
    // Directional Light
    float3 dirLightDir = normalize(-DirectionalLight.Direction);
    float dirDiffuse = saturate(dot(worldNormal, dirLightDir));
    diffuseSum += DirectionalLight.Color * DirectionalLight.Intensity * dirDiffuse;
    
    // Point Lights
    uint lightCount;
    uint pStride;
    PointLights.GetDimensions(lightCount, pStride);
    for (int i = 0; i < lightCount; ++i)
    {
        float3 toLight = PointLights[i].Position - worldPosition;
        float distance = length(toLight);
        if (distance > PointLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float attenuation = ComputeDistanceAttenuation(distance,
            PointLights[i].AttenuationRadius,
            PointLights[i].LightFalloffExponent);
        
        float diffuse = saturate(dot(worldNormal, lightDir));
        diffuseSum += PointLights[i].Color * PointLights[i].Intensity 
            * diffuse * attenuation;
    }

    // Spot Lights
    SpotLights.GetDimensions(lightCount, pStride);
    for (int i = 0; i < lightCount; ++i)
    {
        float3 toLight = SpotLights[i].Position - worldPosition;
        float distance = length(toLight);
        if (distance > SpotLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float attenuation = ComputeDistanceAttenuation(distance,
            SpotLights[i].AttenuationRadius,
            SpotLights[i].LightFalloffExponent);
        
        float spotFactor = ComputeSpotlightFactor(lightDir,
            SpotLights[i].Direction,
            SpotLights[i].InnerConeAngle,
            SpotLights[i].OuterConeAngle);

        float diffuse = saturate(dot(worldNormal, lightDir));
        diffuseSum += SpotLights[i].Color * SpotLights[i].Intensity 
            * diffuse * attenuation * spotFactor;
    }

    diffuseSum = diffuseSum;
    
    return ambientSum + diffuseSum;
}


float3 CalculateBlinnPhongLighting(float3 worldPosition, float3 worldNormal)
{ 
    float3 viewDir = normalize(CameraPosition - worldPosition);
    float3 ambientSum = AmbientLight.Color * AmbientLight.Intensity * Material.AmbientColor;
    float3 diffuseSum = float3(0.0f, 0.0f, 0.0f);
    float3 specularSum = float3(0.0f, 0.0f, 0.0f);
    
    // Directional Light
    float3 dirLightDir = normalize(-DirectionalLight.Direction);
    
    LightFactors factor =  ComputeBlinnPhong(DirectionalLight.Color, 
        DirectionalLight.Intensity, 
        dirLightDir, 
        viewDir, 
        worldNormal, 
        1.0f);
    
    diffuseSum += factor.Diffuse;
    specularSum += factor.Specular;

    uint lightCount;
    uint pStride;
    PointLights.GetDimensions(lightCount, pStride);
    for(int i = 0; i < lightCount; ++i)
    {
        float3 toLight = PointLights[i].Position - worldPosition;
        float distance = length(toLight);
        float3 lightDir = normalize(toLight);
        float attenuation = ComputeDistanceAttenuation(distance,
            PointLights[i].AttenuationRadius,
            PointLights[i].LightFalloffExponent);
        
        
        LightFactors factor = ComputeBlinnPhong(PointLights[i].Color,
        PointLights[i].Intensity,
        lightDir,
        viewDir,
        worldNormal,
        attenuation);
    
        diffuseSum += factor.Diffuse;
        specularSum += factor.Specular;
    }

    SpotLights.GetDimensions(lightCount, pStride);
    for(int i = 0; i < lightCount; ++i)
    {
        float3 toLight = SpotLights[i].Position - worldPosition;
        float distance = length(toLight);
        float3 lightDir = normalize(toLight);
        float attenuation = ComputeDistanceAttenuation(distance,
            SpotLights[i].AttenuationRadius,
            SpotLights[i].LightFalloffExponent);
        
        float spotFactor = ComputeSpotlightFactor(lightDir,
            SpotLights[i].Direction,
            SpotLights[i].InnerConeAngle,
            SpotLights[i].OuterConeAngle);
        
        LightFactors factor = ComputeBlinnPhong(SpotLights[i].Color,
        SpotLights[i].Intensity,
        lightDir,
        viewDir,
        worldNormal,
        attenuation);
    
        diffuseSum += factor.Diffuse * spotFactor;
        specularSum += factor.Specular * spotFactor;
    }

    diffuseSum = diffuseSum;
    specularSum = specularSum * Material.SpecularColor;
    return (ambientSum + diffuseSum + specularSum);
}

float3 CalculateNormalFromMap(float3 baseNormal, float2 texCoord, float3x3 tbn)
{
    float3 resultNormal = baseNormal;
    
    if (Material.TextureSlotMask & (1 << 3))
    {
        float3x3 MInverse3x3 = float3x3(
            MInverseTranspose._11, MInverseTranspose._12, MInverseTranspose._13,
            MInverseTranspose._21, MInverseTranspose._22, MInverseTranspose._23,
            MInverseTranspose._31, MInverseTranspose._32, MInverseTranspose._33
        );
        
        float3 normalMap = BumpTexture.Sample(Sampler, texCoord).rgb;
        normalMap = normalMap * 2.0f - 1.0f;
        
        resultNormal = normalize(mul(mul(normalMap, tbn), MInverse3x3));
    }
    
    return resultNormal;
}

VS_OUT Uber_VS(VS_IN input)
{
    VS_OUT output;
    
    float4 worldPosition = mul(float4(input.position, 1), World);
    output.worldPos = worldPosition.xyz;
    output.position = mul(float4(input.position, 1.0), GetMVP());
    output.normal = normalize(mul(input.normal, (float3x3) MInverseTranspose));
    output.texcoord = input.texcoord;

#if defined(LIGHTING_MODEL_GOURAUD)
    output.color = CalculateGouraudLighting(output.worldPos, output.normal);
    return output;
#endif
    
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

PS_OUT Uber_PS(VS_OUT Input) : SV_TARGET
{
    PS_OUT output;

    // Diffuse 샘플링 및 텍스처 유무 판단
    float3 baseColor = Material.DiffuseColor;
    if (Material.TextureSlotMask & (1 << 0))
    {
        baseColor = DiffuseTexture.Sample(Sampler, Input.texcoord).rgb;
    }
    float3 lighting = float3(1, 1, 1); // 기본 광량 (Unlit fallback)
    float3 normal = Input.normal;
 
#if defined(LIGHTING_MODEL_GOURAUD)
    lighting = Input.color * baseColor;
#elif defined(LIGHTING_MODEL_LAMBERT)
    normal = CalculateNormalFromMap(Input.normal, Input.texcoord, Input.tbn);
    lighting = CalculateLambertLighting(Input.worldPos, normalize(normal));
#elif defined(LIGHTING_MODEL_PHONG)
    normal = CalculateNormalFromMap(Input.normal, Input.texcoord, Input.tbn);
    lighting = CalculateBlinnPhongLighting(Input.worldPos, normalize(normal));
#else
    output.color = float4(baseColor, 1.0f);
    return output;
#endif
    float3 finalColor = baseColor * lighting + Material.EmissiveColor;

    output.color = float4(finalColor, 1.0f);
    
    return output;
}

