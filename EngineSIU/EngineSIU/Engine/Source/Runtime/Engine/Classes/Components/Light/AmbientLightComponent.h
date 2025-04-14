#pragma once

#include "LightComponent.h"

class UAmbientLightComponent : public ULightComponent
{
    DECLARE_CLASS(UAmbientLightComponent, ULightComponent)

public:
    UAmbientLightComponent() = default;

    virtual ELightType GetLightType() const override { return ELightType::Ambient; }

    virtual void UploadLightInfo(void* OutInfo) const override
    {
        FAmbientLightInfo* Info = reinterpret_cast<FAmbientLightInfo*>(OutInfo);
        Info->Color = GetDiffuseColor().ToFVector();
        Info->Intensity = GetIntensity();
    }

    virtual UObject* Duplicate(UObject* InOuter) override
    {
        ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
        //NewComponent->SetLight(GetLight());
        return NewComponent;
    }
};
