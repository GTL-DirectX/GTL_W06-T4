#pragma once

#include "LightComponent.h"

class UDirectionalLightComponent : public ULightComponent
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponent)

public:
    UDirectionalLightComponent() = default;

    virtual ELightType GetLightType() const override { return ELightType::Directional; }

    virtual void UploadLightInfo(void* OutInfo)const override
    {
        FDirectionalLightInfo* Info = reinterpret_cast<FDirectionalLightInfo*>(OutInfo);
        Info->Color = GetDiffuseColor().ToFVector();
        Info->Intensity = GetIntensity();
        Info->Direction = -GetForwardVector();
    }

    virtual UObject* Duplicate(UObject* InOuter) override
    {
        ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
        NewComponent->SetLight(GetLight());
        return NewComponent;
    }
};
