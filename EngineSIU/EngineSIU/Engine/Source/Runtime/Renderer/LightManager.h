#pragma once

#include "LightType.h"
#include "Container/Array.h"

class UPrimitiveDrawBatch;
class FDXDBufferManager;

class FLightManager
{
public:
    void Initialize(FDXDBufferManager* InBufferManager);

    // GPU로 상수버퍼 업로드
    void UpdateLightBuffer();
    void VisualizeLights(UPrimitiveDrawBatch* PrimitiveBatch);

private:
    void CollectLights();

private:
    FDXDBufferManager* BufferManager = nullptr;

    FAmbientLightInfo AmbientLightInfo;
    FDirectionalLightInfo DirectionalLightInfo;
    TArray<FPointLightInfo> PointLights;
    TArray<FSpotLightInfo> SpotLights;
};
