#include "AmbientLightComponent.h"

UObject* UAmbientLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    return NewComponent;
}
