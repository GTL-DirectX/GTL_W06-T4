#include "LightManager.h"

#include "Components/Light/LightComponent.h"
#include "UObject/UObjectIterator.h"

#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"
#include "GameFrameWork/Actor.h"
#include "Math/JungleMath.h"
#include "UnrealEd/EditorViewportClient.h"

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

void FLightManager::UpdateLightBuffer(const std::shared_ptr<FEditorViewportClient>& ActiveViewport)
{
    CollectLights();
    FVector CameraPos = ActiveViewport->ViewTransformPerspective.GetLocation();
    CullLightsByDistance(CameraPos,/*ActiveViewport->ViewFOV*/500.0f);
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
void FLightManager::VisualizeLights(UPrimitiveDrawBatch* PrimitiveBatch)
{
    for (ULightComponent* Light : TObjectRange<ULightComponent>())
    {
        if (!Light->IsVisible())
            continue;

        const FVector Origin = Light->GetWorldLocation();
        const FVector Dir = Light->GetForwardVector();

        if (auto* DirLight = Cast<UDirectionalLightComponent>(Light))
        {
            const FVector Origin = Light->GetWorldLocation();
            const FVector Dir = Light->GetForwardVector();

            // Apex를 전방으로 1만큼 이동한 위치로 설정
            const FVector Apex = Origin + Dir * 1.0f;

            // Height = 거리, Radius는 적당히
            const float Height = -FVector::Distance(Apex, Origin);
            const float Radius = 0.2f; // 얇고 날카로운 Cone

            FMatrix Rotation = FMatrix::GetRotationMatrix(Light->GetWorldRotation());
            PrimitiveBatch->AddConeToBatch(
                Apex,         // Apex: 앞쪽
                Radius,       // Radius: 날카롭게
                Height,       // Height: Origin까지 거리
                16,           // Segment Count
                FVector4(1.0f, 1.0f, 0.2f, 1.0f), // 노란색
                Rotation
            );
        }



        else if (auto* SpotLight = Cast<USpotLightComponent>(Light))
        {
            const FVector Origin = Light->GetWorldLocation();
            const FRotator RotationRot = Light->GetWorldRotation();
            const FMatrix Rotation = FMatrix::GetRotationMatrix(RotationRot);

            const float Range = SpotLight->GetAttenuationRadius();
            const float OuterAngle = SpotLight->GetOuterConeAngle();
            const float InnerAngle = SpotLight->GetInnerConeAngle();

            const float OuterRadius = Range * FMath::Tan(OuterAngle);
            const float InnerRadius = Range * FMath::Tan(InnerAngle);
            const float Height = Range;

            // Outer Cone (붉은색)
            PrimitiveBatch->AddConeToBatch(
                Origin,
                OuterRadius,
                Height,
                16,
                FVector4(1.0f, 0.3f, 0.3f, 1.0f),
                Rotation
            );

            // Inner Cone (노란빛 중심 영역)
            PrimitiveBatch->AddConeToBatch(
                Origin,
                InnerRadius,
                Height,
                16,
                FVector4(1.0f, 1.0f, 0.1f, 1.0f),
                Rotation
            );
        }

    }
}
void FLightManager::CullLightsByDistance(const FVector& ViewOrigin, float FarPlaneDistance)
{
    int OriginalPointCount = PointLights.Num();
    int OriginalSpotCount = SpotLights.Num();
    struct FPointLightWithScore
    {
        float Score;
        FPointLightInfo Info;
    };
    struct FSpotLightWithScore
    {
        float Score;
        FSpotLightInfo Info;
    };

    TArray<FPointLightWithScore> PointLightPool;
    for (const auto& Info : PointLights)
    {
        float dist = FVector::Distance(ViewOrigin, Info.Position);
        if (dist > FarPlaneDistance) continue; // 🚫 FarPlane 밖이면 제외
        float score = FMath::Max(dist - Info.AttenuationRadius, 0.0f); // 📌 영향 범위 고려
        PointLightPool.Add({ score, Info });
    }

    TArray<FSpotLightWithScore> SpotLightPool;
    for (const auto& Info : SpotLights)
    {
        float dist = FVector::Distance(ViewOrigin, Info.Position);
        if (dist > FarPlaneDistance) continue; // 🚫 FarPlane 밖이면 제외
        float score = FMath::Max(dist - Info.AttenuationRadius, 0.0f); // 📌 영향 범위 고려
        SpotLightPool.Add({ score, Info });
    }

    PointLightPool.Sort([](const auto& A, const auto& B) { return A.Score < B.Score; });
    SpotLightPool.Sort([](const auto& A, const auto& B) { return A.Score < B.Score; });

    constexpr int MaxPoint = MAX_POINT_LIGHTS;
    constexpr int MaxSpot = MAX_SPOT_LIGHTS;

    PointLights.Empty();
    for (int i = 0; i < FMath::Min(MaxPoint, PointLightPool.Num()); ++i)
        PointLights.Add(PointLightPool[i].Info);

    SpotLights.Empty();
    for (int i = 0; i < FMath::Min(MaxSpot, SpotLightPool.Num()); ++i)
        SpotLights.Add(SpotLightPool[i].Info);
    UE_LOG(LogLevel::Display, TEXT("Culled PointLights: %d => %d"), OriginalPointCount, PointLights.Num());
    UE_LOG(LogLevel::Display, TEXT("Culled SpotLights:  %d => %d"), OriginalSpotCount, SpotLights.Num());

}
