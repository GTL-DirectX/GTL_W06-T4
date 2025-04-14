#pragma once
#include "GameFramework/Actor.h"

class ULightComponent;
enum class ELightType : uint8;
class UBillboardComponent;

class ALight :public AActor
{
    DECLARE_CLASS(ALight, AActor)
public:
    ALight();
    virtual ~ALight();


    void SetLightType(ELightType InType);
    ELightType GetLightType() const { return LightType; }

    ULightComponent* GetLightComponent() const { return LightComponent; }
protected:
    ELightType LightType;
    UPROPERTY
    (ULightComponent*, LightComponent, = nullptr);

   UPROPERTY
   (UBillboardComponent*, BillboardComponent, = nullptr);
};
