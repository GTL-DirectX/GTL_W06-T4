#pragma once

#include "LightComponent.h"

class UPointLightComponent : public ULightComponent
{
    DECLARE_CLASS(UPointLightComponent, ULightComponent)

public:
    UPointLightComponent() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;
    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;
    
    
    float GetAttenuationRadius() const { return AttRadius; }
    void SetAttenuationRadius(float InRadius) { AttRadius = InRadius; }

    float GetFalloff() const { return Falloff; }
    void SetFalloff(float InFalloff) { Falloff = InFalloff; }
private:
    float AttRadius = 100.f;
    float Falloff = 2.f;
};
