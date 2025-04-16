#include "LightComponentBase.h"


ULightComponentBase::ULightComponentBase()
{
}

void ULightComponentBase::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);

    OutProperties.Add(TEXT("LightDiffuseColor"), *GetDiffuseColor().ToString());
    OutProperties.Add(TEXT("LightSpecularColor"), *GetSpecularColor().ToString());
    OutProperties.Add(TEXT("LightIntensity"), FString::Printf(TEXT("%f"), GetIntensity()));
}

void ULightComponentBase::SetProperties(const TMap<FString, FString>& Properties)
{
    Super::SetProperties(Properties);
    const FString* TempStr = nullptr;

    TempStr = Properties.Find(TEXT("LightDiffuseColor"));
    if (TempStr)
    {
        FLinearColor DiffuseColor = FLinearColor(*TempStr);
        SetDiffuseColor(DiffuseColor);
    }
    TempStr = Properties.Find(TEXT("LightSpecularColor"));
    if (TempStr)
    {
        FLinearColor SpecularColor = FLinearColor(*TempStr);
        SetSpecularColor(SpecularColor);
    }
    TempStr = Properties.Find(TEXT("LightIntensity"));
    if (TempStr)
    {
        float Intensity = FCString::Atof(**TempStr);
        SetIntensity(Intensity);
    }
}

UObject* ULightComponentBase::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    // Light 데이터 복사
    NewComponent->SetLight(this->GetLight());

    return NewComponent;
}
