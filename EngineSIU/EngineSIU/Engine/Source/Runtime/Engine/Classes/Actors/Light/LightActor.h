#pragma once
#include "GameFramework/Actor.h"

class ULightComponent;
class UBillboardComponent;

class ALight :public AActor
{
    DECLARE_CLASS(ALight, AActor)
public:
    ALight();

    ULightComponent* GetLightComponent() const { return LightComponent; }
protected:
    UPROPERTY
    (ULightComponent*, LightComponent, = nullptr);

    UPROPERTY
    (UBillboardComponent*, BillboardComponent, = nullptr);
};
