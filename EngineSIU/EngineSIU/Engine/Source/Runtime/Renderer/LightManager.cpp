#include "LightManager.h"

#include "Components/Light/LightComponent.h"
#include "UObject/UObjectIterator.h"

void FLightManager::Initialize(FDXDBufferManager* InBufferManager)
{
    BufferManager = InBufferManager;
}

void FLightManager::CollectLights()
{
    // 초기화
    AmbientLight = {};
    DirectionalLight = {};
    PointLights.Empty();
    SpotLights.Empty();

    for (ULightComponent* Light : TObjectRange<ULightComponent>())
    {
        if (!Light->IsVisible())
            continue;

        switch (Light->GetLightType())
        {
        case ELightType::Ambient:
            Light->UploadLightInfo(&AmbientLight);
            break;

        case ELightType::Directional:
            Light->UploadLightInfo(&DirectionalLight);
            break;

        case ELightType::Point:
        {
            FPointLightInfo Info = {};
            Light->UploadLightInfo(&Info);
            PointLights.Add(Info);
            break;
        }

        case ELightType::Spot:
        {
            FSpotLightInfo Info = {};
            Light->UploadLightInfo(&Info);
            SpotLights.Add(Info);
            break;
        }
        }
    }
}

void FLightManager::UpdateLightBuffer()
{
    CollectLights();

    FLightBuffer LightBuffer = {};
    LightBuffer.AmbientLight = AmbientLight;
    LightBuffer.DirectionalLight = DirectionalLight;

    for (int i = 0; i < FMath::Min(PointLights.Num(), MAX_POINT_LIGHTS); ++i)
    {
        LightBuffer.PointLights[i] = PointLights[i];
    }

    for (int i = 0; i < FMath::Min(SpotLights.Num(), MAX_SPOT_LIGHTS); ++i)
    {
        LightBuffer.SpotLights[i] = SpotLights[i];
    }

    BufferManager->UpdateConstantBuffer(TEXT("FLightBuffer"), LightBuffer);
}
