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

// 환경광
struct alignas(16) FAmbientLightInfo
{
    FVector Color;
    float Intensity;
};

// 방향광
struct alignas(16) FDirectionalLightInfo
{
    FVector Color;
    float Intensity;

    FVector Direction;
    float Pad = 0.0f;
};

// 점광원
struct alignas(16) FPointLightInfo
{
    FVector Color;
    float Intensity;

    FVector Position;
    float AttenuationRadius;

    float LightFalloffExponent;
    FVector Pad; // align 16
};

// 스포트라이트
struct alignas(16) FSpotLightInfo
{
    FVector Color;
    float Intensity;

    FVector Position;
    float AttenuationRadius;

    float LightFalloffExponent;
    FVector Direction;

    float InnerConeAngle = 0.2f;
    float OuterConeAngle = 0.6f;
    FVector2D Pad; // align 16
};
// 최종 GPU용 조합 구조체
struct FLightBuffer
{
    FAmbientLightInfo AmbientLightInfo;
    FDirectionalLightInfo DirectionalLightInfo;
    FPointLightInfo PointLightInfos[MAX_POINT_LIGHTS];
    FSpotLightInfo SpotLightInfos[MAX_SPOT_LIGHTS];
};
