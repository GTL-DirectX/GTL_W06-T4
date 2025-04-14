#pragma once

#include "LightComponent.h"

class UAmbientLightComponent : public ULightComponent
{
    DECLARE_CLASS(UAmbientLightComponent, ULightComponent)

public:
    UAmbientLightComponent() = default;

    virtual ELightType GetLightType() const override { return ELightType::Ambient; }

    virtual void UploadLightInfo(void* OutInfo) const override;

    virtual UObject* Duplicate(UObject* InOuter) override;
};
