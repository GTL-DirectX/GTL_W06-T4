#include "SpotLight.h"

#include "Components/Light/SpotLightComponent.h"
#include "Components/BillboardComponent.h"

ASpotLight::ASpotLight()
{
    BillboardComponent->SetTexture(L"Assets/Editor/Icon/S_LightSpot.DDS");
    LightComponent = AddComponent<USpotLightComponent>("SpotLight_0");
    SpotLightComponent = Cast<USpotLightComponent>(LightComponent);
}
