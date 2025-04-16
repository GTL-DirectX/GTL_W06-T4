#pragma once
#include "LightComponentBase.h"

class UBillboardComponent;

class ULightComponent : public ULightComponentBase
{
    DECLARE_ABSTRACT_CLASS(ULightComponent, ULightComponentBase)

public:
    ULightComponent();
    virtual ~ULightComponent() override;
    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance) override;
    void InitializeLight();

    bool IsVisible() const { return bVisible; }
protected:

    FBoundingBox AABB;
    bool bVisible = true;

public:
    FBoundingBox GetBoundingBox() const {return AABB;}
};
