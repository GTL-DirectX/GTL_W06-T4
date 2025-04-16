#include "SpotLightComponent.h"

UObject* USpotLightComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    //NewComponent->SetLight(GetLight());
    NewComponent->AttRadius = AttRadius;
    NewComponent->Falloff = Falloff;
    NewComponent->InnerConeAngle = InnerConeAngle;
    NewComponent->OuterConeAngle = OuterConeAngle;
    return NewComponent;
}

void USpotLightComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    ULightComponent::GetProperties(OutProperties);
    OutProperties.Add(TEXT("AttenuationRadius"), FString::Printf(TEXT("%f"), AttRadius));
    OutProperties.Add(TEXT("Falloff"), FString::Printf(TEXT("%f"), Falloff));
    OutProperties.Add(TEXT("InnerConeAngle"), FString::Printf(TEXT("%f"), InnerConeAngle));
    OutProperties.Add(TEXT("OuterConeAngle"), FString::Printf(TEXT("%f"), OuterConeAngle));
}

void USpotLightComponent::SetProperties(const TMap<FString, FString>& InProperties)
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
    TempStr = InProperties.Find(TEXT("InnerConeAngle"));
    if (TempStr)
    {
        InnerConeAngle = FCString::Atof(**TempStr);
    }
    TempStr = InProperties.Find(TEXT("OuterConeAngle"));
    if (TempStr)
    {
        OuterConeAngle = FCString::Atof(**TempStr);
    }
}
