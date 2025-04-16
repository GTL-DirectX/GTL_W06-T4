#include "PointLight.h"

#include "Components/Light/PointLightComponent.h"

APointLight::APointLight()
{
    LightComponent = AddComponent<UPointLightComponent>("PointLight_0");
    PointLightComponent = Cast<UPointLightComponent>(LightComponent);
}
