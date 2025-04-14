#include "LightActor.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/SpotLightComponent.h"

ALight::ALight()
{
    BillboardComponent = AddComponent<UBillboardComponent>();
    BillboardComponent->SetTexture(L"Assets/Editor/Icon/PointLight_64x.png");
    SetRootComponent(BillboardComponent);

    LightType = ELightType::Point; // 기본 타입은 .cpp에서 설정
    SetLightType(LightType);       // 실제 LightComponent 생성
}

ALight::~ALight()
{
}
void ALight::SetLightType(ELightType InType)
{
    LightType = InType;

    // 기존 컴포넌트 제거
    if (LightComponent)
    {
        LightComponent->DestroyComponent();
        LightComponent = nullptr;
    }

    switch (InType)
    {
    case ELightType::Point:
        LightComponent = AddComponent<UPointLightComponent>();
        break;
    case ELightType::Spot:
        LightComponent = AddComponent<USpotLightComponent>();
        break;
    case ELightType::Directional:
        LightComponent = AddComponent<UDirectionalLightComponent>();
        break;
    case ELightType::Ambient:
        LightComponent = AddComponent<UAmbientLightComponent>();
        break;
    default:
        break;
    }

    if (LightComponent)
    {
        LightComponent->AttachToComponent(RootComponent);
        LightComponent->SetLight(FLight(FLinearColor::White, FLinearColor::White, 1000.f));
    }
}
