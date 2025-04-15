#include "LightManager.h"

#include "Components/Light/LightComponent.h"
#include "UObject/UObjectIterator.h"

#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"

void FLightManager::Initialize(FDXDBufferManager* InBufferManager)
{
    BufferManager = InBufferManager;
}

void FLightManager::CollectLights()
{
    // 초기화
    AmbientLightInfo = {};
    DirectionalLightInfo = {};
    PointLights.Empty();
    SpotLights.Empty();

    for (ULightComponent* Light : TObjectRange<ULightComponent>())
    {
        if (!Light->IsVisible())
            continue;

        if (UAmbientLightComponent* AmbientLight = Cast<UAmbientLightComponent>(Light))
        {
            AmbientLight->UploadLightInfo(&AmbientLightInfo);
            continue;
        }
        else if (UDirectionalLightComponent* DirectionalLight = Cast<UDirectionalLightComponent>(Light))
        {
            DirectionalLight->UploadLightInfo(&DirectionalLightInfo);
            continue;
        }
        else if (UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light))
        {
            FPointLightInfo Info = {};
            PointLight->UploadLightInfo(&Info);
            PointLights.Add(Info);
            continue;
        }
        else if (USpotLightComponent* SpotLight = Cast<USpotLightComponent>(Light))
        {
            FSpotLightInfo Info = {};
            SpotLight->UploadLightInfo(&Info);
            SpotLights.Add(Info);
            continue;
        }
    }
}

void FLightManager::UpdateLightBuffer()
{
    CollectLights();

    FLightBuffer LightBuffer = {};
    LightBuffer.AmbientLightInfo = AmbientLightInfo;
    LightBuffer.DirectionalLightInfo = DirectionalLightInfo;

    for (int i = 0; i < FMath::Min(PointLights.Num(), MAX_POINT_LIGHTS); ++i)
    {
        LightBuffer.PointLightInfos[i] = PointLights[i];
    }

    for (int i = 0; i < FMath::Min(SpotLights.Num(), MAX_SPOT_LIGHTS); ++i)
    {
        LightBuffer.SpotLightInfos[i] = SpotLights[i];
    }

    BufferManager->UpdateConstantBuffer(TEXT("FLightBuffer"), LightBuffer);
}
