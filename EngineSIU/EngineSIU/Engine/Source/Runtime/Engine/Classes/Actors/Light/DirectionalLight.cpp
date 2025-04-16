#include "DirectionalLight.h"
#include "Components/Light/DirectionalLightComponent.h"

#include "Components/BillboardComponent.h"

ADirectionalLight::ADirectionalLight()
{
    BillboardComponent->SetTexture(L"Assets/Editor/Icon/S_LightDirectional.DDS");
    LightComponent = AddComponent<UDirectionalLightComponent>("DirectionalLight_0");
    DirectionalLightComponent = Cast<UDirectionalLightComponent>(LightComponent);
}
