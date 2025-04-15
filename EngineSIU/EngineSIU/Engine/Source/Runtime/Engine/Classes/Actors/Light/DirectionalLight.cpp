#include "DirectionalLight.h"
#include "Components/Light/DirectionalLightComponent.h"

ADirectionalLight::ADirectionalLight()
{
    LightComponent = AddComponent<UDirectionalLightComponent>();
    DirectionalLightComponent = Cast<UDirectionalLightComponent>(LightComponent);
}
