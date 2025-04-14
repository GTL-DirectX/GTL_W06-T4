// MatrixBuffer: 변환 행렬 관리
/*
cbuffer MatrixConstants : register(b0)
{
    row_major float4x4 Model;
    row_major float4x4 MInverseTranspose;
    float4 UUID;
    bool isSelected;
    float3 MatrixPad0;
};

cbuffer CameraConstants : register(b1)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
    float3 CameraPosition;
    float pad;
};

#define MAX_LIGHTS 16 

#define POINT_LIGHT         1
#define SPOT_LIGHT          2

struct LIGHT
{
    float3 m_cDiffuse;
    float pad2;

    float3 m_cSpecular;
    float pad3;

    float3 m_vPosition;
    float m_fFalloff; // 스팟라이트의 감쇠 인자

    float3 m_vDirection;
    float pad4;

    float m_fAttenuation; // 거리 기반 감쇠 계수
    int m_bEnable;
    int m_nType;
    float m_fIntensity; // 광원 강도
    
    float m_fAttRadius; // 감쇠 반경 (Attenuation Radius)
    float3 LightPad;
};

cbuffer cbLights : register(b2)
{
    LIGHT gLights[MAX_LIGHTS];
    float4 gcGlobalAmbientLight;
    int gnLights;
    float3 padCB;
};

struct VS_INPUT
{
    float3 position : POSITION; // 버텍스 위치
    float3 normal : NORMAL; // 버텍스 노멀
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR; // 버텍스 색상
    int materialIndex : MATERIAL_INDEX;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 클립 공간으로 변환된 화면 좌표
    float3 worldPos : TEXCOORD0; // 월드 공간 위치 (조명용)
    float4 color : COLOR; // 버텍스 컬러 또는 머티리얼 베이스 컬러
    float3 normal : NORMAL; // 월드 공간 노멀
    float normalFlag : TEXCOORD1; // 노멀 유효 플래그 (1.0 또는 0.0)
    float2 texcoord : TEXCOORD2; // UV 좌표
    int materialIndex : MATERIAL_INDEX; // 머티리얼 인덱스
    float3x3 tbn : TBN_MATRIX;
};

float4 GouraudLight(float3 vPosition, float3 vNormal)
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    [unroll(MAX_LIGHTS)]
    for (int i = 0; i < gnLights; i++)
    {
        if (gLights[i].m_bEnable)
        {
            if (gLights[i].m_nType == POINT_LIGHT)
            {
                float3 lightDir = gLights[i].m_vPosition - vPosition;
                float fDistance = length(lightDir);

                if (fDistance > gLights[i].m_fAttRadius)
                {
                    continue;
                }

                lightDir = normalize(lightDir);
            
                // 거리에 따른 감쇠 계산
                float attenuation = 1.0f - saturate(fDistance / gLights[i].m_fAttRadius);
            
                // 디퓨즈 조명 계산
                float diffuseIntensity = max(dot(vNormal, lightDir), 0.0f);
            
                // 이 조명의 기여도 합산
                cColor += float4(gLights[i].m_cDiffuse * diffuseIntensity * attenuation, 1.0f);
            }
            else if (gLights[i].m_nType == SPOT_LIGHT)
            {
                // cColor += SpotLight(i, vPosition, vNormal);
            }
        }
    }
    
    // 전역 환경광 추가
    cColor += gcGlobalAmbientLight;
    cColor.a = 1;
    
    return cColor;
}

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    output.materialIndex = input.materialIndex;
    
    float4 worldPosition = mul(float4(input.position, 1), Model);
    
    output.worldPos = worldPosition.xyz;
    
    float4 viewPosition = mul(worldPosition, View);
    
    output.position = mul(viewPosition, Projection);
  
    output.normal = normalize(mul(input.normal, (float3x3) MInverseTranspose));
    
    output.color = GouraudLight(worldPosition, output.normal);
    
    float3 biTangent = cross(input.normal, input.tangent);
    
    float3x3 tbn =
    {
        input.tangent.x, input.tangent.y, input.tangent.z, 
        biTangent.x, biTangent.y, biTangent.z, 
        input.normal.x, input.normal.y, input.normal.z 
    };
    
    output.tbn = tbn;
        
    output.texcoord = input.texcoord;
    
    return output;
}

*/
