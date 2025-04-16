#pragma once
#include "LightComponent.h"

class USpotLightComponent : public ULightComponent
{
    DECLARE_CLASS(USpotLightComponent, ULightComponent)

public:
    USpotLightComponent() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;
    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;

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
