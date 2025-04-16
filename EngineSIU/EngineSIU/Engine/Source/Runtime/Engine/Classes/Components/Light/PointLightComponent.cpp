#include "PointLightComponent.h"

UObject* UPointLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    NewComponent->AttRadius = AttRadius;
    NewComponent->Falloff = Falloff;
    return NewComponent;
}
