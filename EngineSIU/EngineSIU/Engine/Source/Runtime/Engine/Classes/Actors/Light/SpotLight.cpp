#include "SpotLight.h"

#include "Components/Light/SpotLightComponent.h"

ASpotLight::ASpotLight()
{
    LightComponent = AddComponent<USpotLightComponent>("SpotLight_0");
    SpotLightComponent = Cast<USpotLightComponent>(LightComponent);
}
