#pragma once

#include "LightType.h"
#include "Container/Array.h"

class FDXDBufferManager;

class FLightManager
{
public:
    void Initialize(FDXDBufferManager* InBufferManager);

    // GPU로 상수버퍼 업로드
    void UpdateLightBuffer();

private:
    void CollectLights();

private:
    FDXDBufferManager* BufferManager = nullptr;

    FAmbientLightInfo AmbientLight;
    FDirectionalLightInfo DirectionalLight;
    TArray<FPointLightInfo> PointLights;
    TArray<FSpotLightInfo> SpotLights;
};
