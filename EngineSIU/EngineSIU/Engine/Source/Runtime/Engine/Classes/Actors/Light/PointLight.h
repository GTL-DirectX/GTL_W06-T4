#pragma once
#include "LightActor.h"

class UPointLightComponent;

class APointLight : public ALight
{
    DECLARE_CLASS(APointLight, ALight)

public:
    APointLight();

private:
    UPointLightComponent* PointLightComponent;
};

