#pragma once

#include "LightComponent.h"

class UDirectionalLightComponent : public ULightComponent
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponent)

public:
    UDirectionalLightComponent() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;
};
