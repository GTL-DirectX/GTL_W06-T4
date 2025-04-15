#include "AmbientLight.h"

#include "Components/Light/AmbientLightComponent.h"

AAmbientLight::AAmbientLight()
{
    LightComponent = AddComponent<UAmbientLightComponent>();
    AmbientLightComponent = Cast<UAmbientLightComponent>(LightComponent);
}
