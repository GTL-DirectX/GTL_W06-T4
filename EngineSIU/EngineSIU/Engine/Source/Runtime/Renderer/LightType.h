#pragma once

// 공통 설정
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS  4
#include "Math/Color.h"
#include "Math/Vector.h"

struct FLight
{
    FLinearColor DiffuseColor=FLinearColor::White;
    FLinearColor SpecularColor;
    float Intensity = 1.0f;

    FLight() = default;
    FLight(const FVector& InDiffuse, const FVector& InSpecular, float InIntensity)
        : DiffuseColor(InDiffuse), SpecularColor(InSpecular), Intensity(InIntensity)
    {
    }
    FLight(const FLinearColor& InDiffuse, const FLinearColor& InSpecular, float InIntensity)
        : DiffuseColor(InDiffuse), SpecularColor(InSpecular), Intensity(InIntensity)
    {
    }
};
// --------------------------------------------------
// GPU에 전달할 라이트 구조체 (상수버퍼용)
// --------------------------------------------------

// 환경광 (Ambient)
struct FAmbientLightInfo
{
    FVector Color;         // RGB 컬러
    float Intensity = 1.0f;
};

// 방향광 (Directional)
struct FDirectionalLightInfo
{
    FVector Color;         // RGB 컬러
    float Intensity = 1.0f;

    FVector Direction;     // 단위 벡터
    float Pad = 0.0f;
};

// 포인트 라이트 (Point)
struct FPointLightInfo
{
    FVector Color;         // RGB 컬러
    float Intensity = 1.0f;

    FVector Position;      // 월드 위치
    float AttenuationRadius = 100.0f;

    float LightFalloffExponent = 2.0f;
    FVector Pad = FVector(); // align 16
};

// 스포트라이트 (Spot)
struct FSpotLightInfo
{
    FVector Color;         // RGB 컬러
    float Intensity = 1.0f;

    FVector Position;      // 월드 위치
    float AttenuationRadius = 100.0f;

    float LightFalloffExponent = 2.0f;
    FVector Direction;     // 단위 벡터

    float InnerConeAngle = 0.2f; // radians
    float OuterConeAngle = 0.6f; // radians
    FVector2D Pad = FVector2D(0.0f, 0.0f);
};

// 최종 GPU용 조합 구조체
struct FLightBuffer
{
    FAmbientLightInfo AmbientLightInfo;
    FDirectionalLightInfo DirectionalLightInfo;
    FPointLightInfo PointLightInfos[MAX_POINT_LIGHTS];
    FSpotLightInfo SpotLightInfos[MAX_SPOT_LIGHTS];
};
