#include "ShaderCommon.hlsl"
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

Texture2D DiffuseTexture : register(t0);
Texture2D AmbientTexture : register(t1);
Texture2D SpecularTexture : register(t2);
Texture2D BumpTexture : register(t3);
Texture2D AlphaTexture : register(t4);
SamplerState Sampler : register(s0);
cbuffer Lighting : register(b0)
{
    FAmbientLightInfo AmbientLight;
    FDirectionalLightInfo DirectionalLight;
    FPointLightInfo PointLights[NUM_POINT_LIGHT];
    FSpotLightInfo SpotLights[NUM_SPOT_LIGHT];
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
float4 CaculateAmbientLight(FAmbientLightInfo InLightInfo)
{
    float4 Result;

    return Result;
}

struct VS_IN
{
    float3 position : POSITION; // 버텍스 위치
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR; // 버텍스 색상
    int materialIndex : MATERIAL_INDEX;
};

struct VS_OUT
{
    float4 position : SV_POSITION; // 클립 공간으로 변환된 화면 좌표
    float3 worldPos : TEXCOORD0; // 월드 공간 위치 (조명용)
    float4 color : COLOR; // 버텍스 컬러 또는 머티리얼 베이스 컬러
    float3 normal : NORMAL; // 월드 공간 노멀
    float normalFlag : TEXCOORD1; // 노멀 유효 플래그 (1.0 또는 0.0)
    float2 texcoord : TEXCOORD2; // UV 좌표
    int materialIndex : MATERIAL_INDEX; // 머티리얼 인덱스
    float3x3 tbn : TBN_MATRIX; // TBN Matrix for normal mapping
};

struct PS_OUT
{
    float4 color : SV_Target0;
};
VS_OUT Uber_VS(VS_IN input)
{
    VS_OUT output;
    
    output.materialIndex = input.materialIndex;
    
    /*
    float4 worldPosition = mul(float4(input.position, 1), World);
    
    output.worldPos = worldPosition.xyz;
    
    float4 viewPosition = mul(worldPosition, View);
    
    output.position = mul(viewPosition, Projection);
    */
    float4 worldPosition = mul(float4(input.position, 1), World);
    output.worldPos = worldPosition.xyz;
    output.position = mul(float4(input.position, 1.0), GetMVP());
    output.color = input.color;
  
    output.normal = normalize(mul(input.normal, (float3x3) MInverseTranspose));
    float3 biTangent = cross(input.normal, input.tangent);
    
    float3x3 tbn =
    {
        input.tangent.x, input.tangent.y, input.tangent.z,
        biTangent.x, biTangent.y, biTangent.z,
        input.normal.x, input.normal.y, input.normal.z 
    };
    output.tbn = tbn;
    
    output.texcoord = input.texcoord;

    //color 일단 제거
#if LIGHTING_MODEL_GOURAUD

#elif LIGHTING_MODEL_LAMBERT

#elif LIGHTING_MODEL_PHONG

#endif
    
    return output;
}
float3 CalculateDefaultLighting(float3 worldPos, float3 normal)
{
    float3 result = AmbientLight.Color * AmbientLight.Intensity * Material.AmbientColor;

    // Directional Light
    {
        float3 lightDir = normalize(-DirectionalLight.Direction);
        float diff = max(dot(normal, lightDir), 0.0f);
        result += DirectionalLight.Color * DirectionalLight.Intensity * Material.DiffuseColor * diff;
    }

    // Point Lights
    for (int i = 0; i < NUM_POINT_LIGHT; ++i)
    {
        float3 toLight = PointLights[i].Position - worldPos;
        float distance = length(toLight);
        if (distance > PointLights[i].AttenuationRadius)
            continue;
        float radius = PointLights[i].AttenuationRadius;
        float exp = PointLights[i].LightFalloffExponent;
        float atten = pow(saturate(1 - distance / radius), exp);
        float3 lightDir = normalize(toLight);

        float diff = max(dot(normal, lightDir), 0.0f);
        result += PointLights[i].Color * PointLights[i].Intensity * Material.DiffuseColor * diff * atten;
    }

    // Spot Lights
    for (int i = 0; i < NUM_SPOT_LIGHT; ++i)
    {
        float3 toLight = SpotLights[i].Position - worldPos;
        float distance = length(toLight);
        if (distance > SpotLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float diff = max(dot(normal, lightDir), 0.0f);
        float3 viewDir = normalize(CameraPosition - worldPos);
        float3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0f), 16);

        float angle = dot(lightDir, normalize(-SpotLights[i].Direction));
        float inner = cos(SpotLights[i].InnerConeAngle);
        float outer = cos(SpotLights[i].OuterConeAngle);
        float spot = smoothstep(outer, inner, angle);
        float att = pow(saturate(1.0f - distance / SpotLights[i].AttenuationRadius), SpotLights[i].LightFalloffExponent);

        result += SpotLights[i].Color * SpotLights[i].Intensity * Material.DiffuseColor * diff * spot * att;
        result += SpotLights[i].Color * Material.SpecularColor * spec * spot * att;
    }

    return result;
}
float3 CalculateLambertLighting(float3 worldPos, float3 normal)
{
    float3 result = AmbientLight.Color * AmbientLight.Intensity * Material.AmbientColor;

    // Directional Light
    {
        float3 lightDir = normalize(-DirectionalLight.Direction);
        float diff = max(dot(normal, lightDir), 0.0f);
        result += DirectionalLight.Color * DirectionalLight.Intensity * Material.DiffuseColor * diff;
    }

    // Point Lights
    for (int i = 0; i < NUM_POINT_LIGHT; ++i)
    {
        float3 toLight = PointLights[i].Position - worldPos;
        float distance = length(toLight);
        if (distance > PointLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float diff = max(dot(normal, lightDir), 0.0f);
        float att = pow(saturate(1.0f - distance / PointLights[i].AttenuationRadius), PointLights[i].LightFalloffExponent);

        result += PointLights[i].Color * PointLights[i].Intensity * Material.DiffuseColor * diff * att;
    }

    // Spot Lights
    for (int i = 0; i < NUM_SPOT_LIGHT; ++i)
    {
        float3 toLight = SpotLights[i].Position - worldPos;
        float distance = length(toLight);
        if (distance > SpotLights[i].AttenuationRadius)
            continue;

        float3 lightDir = normalize(toLight);
        float diff = max(dot(normal, lightDir), 0.0f);
        float att = pow(saturate(1.0f - distance / SpotLights[i].AttenuationRadius), SpotLights[i].LightFalloffExponent);

        float angle = dot(lightDir, normalize(-SpotLights[i].Direction));
        float spotFactor = smoothstep(SpotLights[i].OuterConeAngle, SpotLights[i].InnerConeAngle, angle);

        result += SpotLights[i].Color * SpotLights[i].Intensity * Material.DiffuseColor * diff * att * spotFactor;
    }

    return result;
}


PS_OUT Uber_PS(VS_OUT Input) : SV_TARGET
{
    PS_OUT output;

    // 1. 알베도 샘플링 및 텍스처 유무 판단
    float3 albedo = Textures.Sample(Sampler, Input.texcoord).rgb;
    //float3 baseColor = any(albedo != float3(0, 0, 0)) ? albedo : Material.DiffuseColor;
    float3 baseColor =  Material.DiffuseColor;
    baseColor = any(Textures.Sample(Sampler, Input.texcoord).rgb != float3(0, 0, 0))
                       ? Textures.Sample(Sampler, Input.texcoord).rgb
                       : Material.DiffuseColor;
    float3 lighting = float3(1, 1, 1); // 기본 광량 (Unlit fallback)
    
#if LIGHTING_MODEL_GOURAUD
    // Gouraud: Vertex에서 조명 계산해 전달받아야 함 (현재 미구현)
    // lighting = Input.Lighting;
    
#elif LIGHTING_MODEL_LAMBERT
    lighting = CalculateLambertLighting(Input.worldPos, normalize(Input.normal));
    
#elif LIGHTING_MODEL_PHONG
    lighting = CalculateDefaultLighting(Input.worldPos, normalize(Input.normal));
    
#else
    // No lighting, 기본 베이스 색상만 적용
    //lighting = float3(1, 1, 1);
    lighting = CalculateDefaultLighting(Input.worldPos, normalize(Input.normal));
#endif

    lighting = saturate(lighting);
    float3 finalColor = baseColor * lighting + Material.EmissiveColor;
    output.color = float4(finalColor, 1.0f);

    

    return output;
}


/*
PS_OUT Uber_PS(VS_OUT Input) : SV_TARGET
{
    PS_OUT output;
    // 1) 알베도 샘플링
    float3 albedo = DiffuseTexture.Sample(Sampler, Input.texcoord).rgb;
    // 2) 머티리얼 디퓨즈
    float3 matDiffuse = Material.DiffuseColor.rgb;
    // 3) 라이트 계산
    float3 normal = Input.normal;

    bool hasTexture = any(albedo != float3(0, 0, 0));

    /*if (IsLit)
    {
        float3 lightRgb = Lighting(Input.worldPos, Input.normal).rgb;
        float3 litColor = baseColor * lightRgb;
        output.color = float4(litColor, 1);
    }
    else
    {
        output.color = float4(baseColor, 1);
        
    }
    if (isSelected)
    {
        output.color += float4(0.02, 0.02, 0.02, 1);

    }#1#

#if LIGHTING_MODEL_GOURAUD

#elif LIGHTING_MODEL_LAMBERT
    {
    float3 lighting = CalculateLambertLighting(Input.worldPos, normalize(Input.normal));
    float3 baseColor = any(Textures.Sample(Sampler, Input.texcoord).rgb != float3(0, 0, 0))
                       ? Textures.Sample(Sampler, Input.texcoord).rgb
                       : Material.DiffuseColor;

    output.color = float4(baseColor * lighting + Material.EmissiveColor, 1.0f);
    }
#elif LIGHTING_MODEL_PHONG
#else
    // Lambert, Gouraud, Phong이 아닌 경우 기본 베이스 색상
    float3 baseColor = hasTexture ? albedo : matDiffuse;
    output.color = float4(baseColor, 1);
#endif
    // 아래 내용을 통해 Normalmapping 사용
    if (Material.TextureSlotMask & (1 << 3))
    {
        float3x3 MInverse3x3 = float3x3(
            MInverseTranspose._11, MInverseTranspose._12, MInverseTranspose._13,
            MInverseTranspose._21, MInverseTranspose._22, MInverseTranspose._23,
            MInverseTranspose._31, MInverseTranspose._32, MInverseTranspose._33
        );
        float3 normalMap = BumpTexture.Sample(Sampler, Input.texcoord).rgb;
        
        normalMap = normalMap * 2.0f - 1.0f;

        normal = normalize(mul(mul(normalMap, Input.tbn), MInverse3x3));
    }
    
    float3 lighting = CalculateLambertLighting(Input.worldPos, normalize(normal));
    lighting = saturate(lighting);
    baseColor = any(DiffuseTexture.Sample(Sampler, Input.texcoord).rgb != float3(0, 0, 0))
                       ? DiffuseTexture.Sample(Sampler, Input.texcoord).rgb
                       : Material.DiffuseColor;
    output.color = float4(baseColor * (lighting + Material.EmissiveColor), 1.0f);

    // 아래 내용은 Normal을 색상으로 표현 시 사용
    // float3 normalColor = (normal + 1.0f) * 0.5f;
    // output.color = float4(normalColor, 1.0f);
    
    //output.color = float4(baseColor * (lighting + float3(0.1, 0.1, 0.1)), 1.0f);
    
    return output;
    //return float4(finalColor, 1.0f);
}
*/

