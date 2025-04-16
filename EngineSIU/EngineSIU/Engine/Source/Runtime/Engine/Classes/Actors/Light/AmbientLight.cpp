#include "AmbientLight.h"

#include "Components/Light/AmbientLightComponent.h"

AAmbientLight::AAmbientLight()
{
    LightComponent = AddComponent<UAmbientLightComponent>("AmbientLight_0");
    AmbientLightComponent = Cast<UAmbientLightComponent>(LightComponent);
}
