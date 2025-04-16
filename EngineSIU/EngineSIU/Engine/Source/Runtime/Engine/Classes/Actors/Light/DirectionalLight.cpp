#include "DirectionalLight.h"
#include "Components/Light/DirectionalLightComponent.h"

ADirectionalLight::ADirectionalLight()
{
    LightComponent = AddComponent<UDirectionalLightComponent>("DirectionalLight_0");
    DirectionalLightComponent = Cast<UDirectionalLightComponent>(LightComponent);
}
