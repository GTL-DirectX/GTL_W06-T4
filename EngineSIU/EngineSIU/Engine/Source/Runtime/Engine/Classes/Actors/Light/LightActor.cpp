#include "LightActor.h"
#include "Components/BillboardComponent.h"

ALight::ALight()
{
    BillboardComponent = AddComponent<UBillboardComponent>("BillboardIcon_0");
    BillboardComponent->SetTexture(L"Assets/Editor/Icon/PointLight_64x.png");
    
}
