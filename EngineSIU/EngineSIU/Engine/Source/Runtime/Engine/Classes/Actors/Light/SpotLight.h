#pragma once
#include "LightActor.h"

class USpotLightComponent;

class ASpotLight : public ALight
{
    DECLARE_CLASS(ASpotLight, ALight)

public:
    ASpotLight();
    
private:
    USpotLightComponent* SpotLightComponent;
};

