#pragma once

#include <variant>

#include "LightType.h"
#include "Container/Array.h"

class USpotLightComponent;
class UPointLightComponent;
class UDirectionalLightComponent;
class UAmbientLightComponent;
class ULightComponent;
class FEditorViewportClient;
class UPrimitiveDrawBatch;
class FDXDBufferManager;

class FLightManager
{
public:
    void Initialize(FDXDBufferManager* InBufferManager);

    // GPU로 상수버퍼 업로드
    void UpdateLightBuffer(const std::shared_ptr<FEditorViewportClient>& ActiveViewport);
    void VisualizeLights(UPrimitiveDrawBatch* PrimitiveBatch);
    void CullLightsByDistance(const FVector& ViewOrigin, float FarPlaneDistance);

    // 라이트 타입 분기 후 Info 반환 (variant로 통합)
    std::variant<FAmbientLightInfo, FDirectionalLightInfo, FPointLightInfo, FSpotLightInfo>
        BuildLightInfo(ULightComponent* Light);

    // 각 라이트 타입별 개별 생성
    FAmbientLightInfo MakeAmbientLightInfo(UAmbientLightComponent* Light);
    FDirectionalLightInfo MakeDirectionalLightInfo(UDirectionalLightComponent* Light);
    FPointLightInfo MakePointLightInfo(UPointLightComponent* Light);
    FSpotLightInfo MakeSpotLightInfo(USpotLightComponent* Light);
private:
    void CollectLights();

private:
    FDXDBufferManager* BufferManager = nullptr;

    FAmbientLightInfo AmbientLightInfo;
    FDirectionalLightInfo DirectionalLightInfo;
    TArray<FPointLightInfo> PointLights;
    TArray<FSpotLightInfo> SpotLights;
};
