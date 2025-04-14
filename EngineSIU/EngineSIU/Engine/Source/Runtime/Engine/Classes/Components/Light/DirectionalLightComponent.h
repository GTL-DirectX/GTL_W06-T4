#pragma once

#include "LightComponent.h"

class UDirectionalLightComponent : public ULightComponent
{
    DECLARE_CLASS(UDirectionalLightComponent, ULightComponent)

public:
    UDirectionalLightComponent() = default;

    virtual void UploadLightInfo(void* OutInfo)const override;

    virtual UObject* Duplicate(UObject* InOuter) override;
};
