#include "AmbientLightComponent.h"

void UAmbientLightComponent::UploadLightInfo(void* OutInfo) const
{
    FAmbientLightInfo* Info = reinterpret_cast<FAmbientLightInfo*>(OutInfo);
    Info->Color = GetDiffuseColor().ToFVector();
    Info->Intensity = GetIntensity();
}

UObject* UAmbientLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    return NewComponent;
}
