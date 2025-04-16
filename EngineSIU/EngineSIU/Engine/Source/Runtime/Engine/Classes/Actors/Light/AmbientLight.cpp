#include "AmbientLight.h"

#include "Components/Light/AmbientLightComponent.h"
#include "Components/BillboardComponent.h"

AAmbientLight::AAmbientLight()
{
    BillboardComponent->SetTexture(L"Assets/Editor/Icon/SkyLight.DDS");
    LightComponent = AddComponent<UAmbientLightComponent>("AmbientLight_0");
    AmbientLightComponent = Cast<UAmbientLightComponent>(LightComponent);
}
