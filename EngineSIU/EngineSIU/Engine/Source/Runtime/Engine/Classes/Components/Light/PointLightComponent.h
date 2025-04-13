#pragma once

#include "LightComponent.h"

class UPointLightComponent : public ULightComponent
{
    DECLARE_CLASS(UPointLightComponent, ULightComponent)

public:
    UPointLightComponent() = default;

    virtual ELightType GetLightType() const override { return ELightType::Point; }

    virtual void UploadLightInfo(void* OutInfo) const override
    {
        FPointLightInfo* Info = reinterpret_cast<FPointLightInfo*>(OutInfo);
        Info->Color = GetDiffuseColor().ToFVector();
        Info->Intensity = GetIntensity();
        Info->Position = GetWorldLocation();
        Info->AttenuationRadius = AttRadius;
        Info->LightFalloffExponent = Falloff;
    }

    virtual UObject* Duplicate(UObject* InOuter) override
    {
        ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
        //NewComponent->SetLight(GetLight());
        NewComponent->AttRadius = AttRadius;
        NewComponent->Falloff = Falloff;
        return NewComponent;
    }
    float GetAttenuationRadius() const { return AttRadius; }
    void SetAttenuationRadius(float InRadius) { AttRadius = InRadius; }

    float GetFalloff() const { return Falloff; }
    void SetFalloff(float InFalloff) { Falloff = InFalloff; }
private:
    float AttRadius = 100.f;
    float Falloff = 2.f;
};
