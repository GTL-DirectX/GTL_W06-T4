#include "PointLight.h"

#include "Components/Light/PointLightComponent.h"

APointLight::APointLight()
{
    LightComponent = AddComponent<UPointLightComponent>();
    PointLightComponent = Cast<UPointLightComponent>(LightComponent);
}
