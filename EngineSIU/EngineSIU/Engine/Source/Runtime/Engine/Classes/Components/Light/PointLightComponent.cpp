#include "PointLightComponent.h"

void UPointLightComponent::UploadLightInfo(void* OutInfo) const
{
    FPointLightInfo* Info = reinterpret_cast<FPointLightInfo*>(OutInfo);
    Info->Color = GetDiffuseColor().ToFVector();
    Info->Intensity = GetIntensity();
    Info->Position = GetWorldLocation();
    Info->AttenuationRadius = AttRadius;
    Info->LightFalloffExponent = Falloff;
}

UObject* UPointLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    NewComponent->AttRadius = AttRadius;
    NewComponent->Falloff = Falloff;
    return NewComponent;
}
