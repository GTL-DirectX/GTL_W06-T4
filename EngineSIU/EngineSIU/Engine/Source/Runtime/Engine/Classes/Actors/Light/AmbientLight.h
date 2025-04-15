#pragma once

#include "LightActor.h"

class UAmbientLightComponent;

class AAmbientLight : public ALight
{
    DECLARE_CLASS(AAmbientLight, ALight)

public:
    AAmbientLight();

private:
    UAmbientLightComponent* AmbientLightComponent;
};

