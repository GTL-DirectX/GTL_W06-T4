#pragma once

#include "Components/SceneComponent.h"

#include "Math/Color.h"
#include "Renderer/LightType.h"
#include "UObject/Casts.h"

class ULightComponentBase : public USceneComponent
{
    DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
    ULightComponentBase();

public:
    virtual UObject* Duplicate(UObject* InOuter)override;
    const FLight& GetLight() const { return LightData; }
    void SetLight(const FLight& InLight) { LightData = InLight; }
    FLinearColor GetSpecularColor() const { return LightData.SpecularColor; }
    void SetSpecularColor(const FLinearColor& InColor) { LightData.SpecularColor = InColor; }
    FLinearColor GetDiffuseColor() const { return LightData.DiffuseColor; }
    void SetDiffuseColor(const FLinearColor& InColor) { LightData.DiffuseColor = InColor; }

    float GetIntensity() const { return LightData.Intensity; }
    void SetIntensity(float InIntensity) { LightData.Intensity = InIntensity; }
    //bool IsVisible() const { return LightData.bVisible; }

protected:
    FLight LightData;

};
