#include "DirectionalLightComponent.h"

void UDirectionalLightComponent::UploadLightInfo(void* OutInfo) const
{
    FDirectionalLightInfo* Info = reinterpret_cast<FDirectionalLightInfo*>(OutInfo);
    Info->Color = GetDiffuseColor().ToFVector();
    Info->Intensity = GetIntensity();
    Info->Direction = -GetForwardVector();
}

UObject* UDirectionalLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    return NewComponent;
}
