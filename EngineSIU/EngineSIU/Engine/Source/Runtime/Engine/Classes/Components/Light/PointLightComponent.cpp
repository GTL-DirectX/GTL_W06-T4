#include "PointLightComponent.h"

UObject* UPointLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    NewComponent->AttRadius = AttRadius;
    NewComponent->Falloff = Falloff;
    return NewComponent;
}

void UPointLightComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    ULightComponent::GetProperties(OutProperties);

    OutProperties.Add(TEXT("AttenuationRadius"), FString::Printf(TEXT("%f"), AttRadius));
    OutProperties.Add(TEXT("Falloff"), FString::Printf(TEXT("%f"), Falloff));
}

void UPointLightComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    ULightComponent::SetProperties(InProperties);
    const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("AttenuationRadius"));
    if (TempStr)
    {
        AttRadius = FCString::Atof(**TempStr);
    }
    TempStr = InProperties.Find(TEXT("Falloff"));
    if (TempStr)
    {
        Falloff = FCString::Atof(**TempStr);
    }
}
