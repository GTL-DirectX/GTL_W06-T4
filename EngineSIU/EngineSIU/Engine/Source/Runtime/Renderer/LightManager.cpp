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

    // structuredBuffer 0번 인덱스용 padding
    FPointLightInfo tempPoint;
    PointLights.Add(tempPoint);
    FSpotLightInfo tempSpot;
    SpotLights.Add(tempSpot);

    for (ULightComponent* Light : TObjectRange<ULightComponent>())
    {
        if (!Light->IsVisible())
            continue;

        if (auto* AmbientLight = Cast<UAmbientLightComponent>(Light))
        {
            AmbientLightInfo = MakeAmbientLightInfo(AmbientLight);
        }
        else if (auto* DirectionalLight = Cast<UDirectionalLightComponent>(Light))
        {
            DirectionalLightInfo = MakeDirectionalLightInfo(DirectionalLight);
        }
        else if (auto* PointLight = Cast<UPointLightComponent>(Light))
        {
            PointLights.Add(MakePointLightInfo(PointLight));
        }
        else if (auto* SpotLight = Cast<USpotLightComponent>(Light))
        {
            SpotLights.Add(MakeSpotLightInfo(SpotLight));
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

    BufferManager->CreateStructuredBuffer("FPointLightInfoStructuredBuffer", PointLights.GetData(), sizeof(FPointLightInfo), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, PointLights.Num());
    BufferManager->CreateStructuredBuffer("FSpotLightInfoStructuredBuffer", SpotLights.GetData(), sizeof(FSpotLightInfo), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, SpotLights.Num());

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
std::variant<FAmbientLightInfo, FDirectionalLightInfo, FPointLightInfo, FSpotLightInfo>
FLightManager::BuildLightInfo(ULightComponent* Light)
{
    if (auto* Ambient = Cast<UAmbientLightComponent>(Light))
        return MakeAmbientLightInfo(Ambient);

    if (auto* Dir = Cast<UDirectionalLightComponent>(Light))
        return MakeDirectionalLightInfo(Dir);

    if (auto* Point = Cast<UPointLightComponent>(Light))
        return MakePointLightInfo(Point);

    if (auto* Spot = Cast<USpotLightComponent>(Light))
        return MakeSpotLightInfo(Spot);


    return FAmbientLightInfo(); // fallback
}

FAmbientLightInfo FLightManager::MakeAmbientLightInfo(UAmbientLightComponent* Light)
{
    return {
        Light->GetDiffuseColor().ToFVector(),
        Light->GetIntensity()
    };
}

FDirectionalLightInfo FLightManager::MakeDirectionalLightInfo(UDirectionalLightComponent* Light)
{
    return {
        Light->GetDiffuseColor().ToFVector(),
        Light->GetIntensity(),
        Light->GetForwardVector(),
        0.0f
    };
}

FPointLightInfo FLightManager::MakePointLightInfo(UPointLightComponent* Light)
{
    return {
        Light->GetDiffuseColor().ToFVector(),
        Light->GetIntensity(),
        Light->GetWorldLocation(),
        Light->GetAttenuationRadius(),
        Light->GetFalloff(),
        FVector::ZeroVector // pad
    };
}

FSpotLightInfo FLightManager::MakeSpotLightInfo(USpotLightComponent* Light)
{
    return {
        Light->GetDiffuseColor().ToFVector(),
        Light->GetIntensity(),
        Light->GetWorldLocation(),
        Light->GetAttenuationRadius(),
        Light->GetFalloff(),
        Light->GetForwardVector(),
        Light->GetInnerConeAngle(),
        Light->GetOuterConeAngle(),
        FVector2D(0, 0)
    };
}
