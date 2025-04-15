#include "SpotLight.h"

#include "Components/Light/SpotLightComponent.h"

ASpotLight::ASpotLight()
{
    LightComponent = AddComponent<USpotLightComponent>();
    SpotLightComponent = Cast<USpotLightComponent>(LightComponent);
}
