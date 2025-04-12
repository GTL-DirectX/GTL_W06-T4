#pragma once
#include "LightComponent.h"

class USpotLightComponent : public ULightComponent
{
    DECLARE_CLASS(USpotLightComponent, ULightComponent)

public:
    USpotLightComponent() = default;

    virtual ELightType GetLightType() const override { return ELightType::Spot; }

    virtual void UploadLightInfo(void* OutInfo) const override
    {
        FSpotLightInfo* Info = reinterpret_cast<FSpotLightInfo*>(OutInfo);
        Info->Color = GetDiffuseColor().ToFVector();
        Info->Intensity = GetIntensity();
        Info->Position = GetWorldLocation();
        Info->Direction = GetForwardVector();
        Info->AttenuationRadius = AttRadius;
        Info->LightFalloffExponent = Falloff;
        Info->InnerConeAngle = InnerConeAngle;
        Info->OuterConeAngle = OuterConeAngle;
    }

    virtual UObject* Duplicate(UObject* InOuter) override
    {
        ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
        NewComponent->SetLight(GetLight());
        NewComponent->AttRadius = AttRadius;
        NewComponent->Falloff = Falloff;
        NewComponent->InnerConeAngle = InnerConeAngle;
        NewComponent->OuterConeAngle = OuterConeAngle;
        return NewComponent;
    }

    float GetAttenuationRadius() const { return AttRadius; }
    void SetAttenuationRadius(float InRadius) { AttRadius = InRadius; }

    float GetFalloff() const { return Falloff; }
    void SetFalloff(float InFalloff) { Falloff = InFalloff; }

    float GetInnerConeAngle() const { return InnerConeAngle; }
    void SetInnerConeAngle(float InAngle) { InnerConeAngle = InAngle; }

    float GetOuterConeAngle() const { return OuterConeAngle; }
    void SetOuterConeAngle(float InAngle) { OuterConeAngle = InAngle; }

private:
    float AttRadius = 100.f;
    float Falloff = 2.f;
    float InnerConeAngle = 0.2f;
    float OuterConeAngle = 0.6f;
};
