#include "LightActor.h"
#include "Components/BillboardComponent.h"

ALight::ALight()
{
    BillboardComponent = AddComponent<UBillboardComponent>();
    BillboardComponent->SetTexture(L"Assets/Editor/Icon/PointLight_64x.png");
}
