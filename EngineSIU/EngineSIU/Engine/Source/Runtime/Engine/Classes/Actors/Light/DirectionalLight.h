#pragma once

#include "LightActor.h"

class UDirectionalLightComponent;

class ADirectionalLight : public ALight
{
    DECLARE_CLASS(ADirectionalLight, ALight)

public:
    ADirectionalLight();

private:
    UDirectionalLightComponent* DirectionalLightComponent;

};

