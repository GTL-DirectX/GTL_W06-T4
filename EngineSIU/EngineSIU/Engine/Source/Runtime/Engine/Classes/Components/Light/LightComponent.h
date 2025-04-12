#pragma once
#include "LightComponentBase.h"
#include "Define.h"
class UBillboardComponent;
enum class ELightType : uint8
{
    Ambient,
    Directional,
    Point,
    Spot
};
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

    virtual ELightType GetLightType() const=0;
    // GPU 업로드용 LightInfo를 추출 (각 하위 클래스에서 오버라이드)
    virtual void UploadLightInfo(void* OutInfo)const {};
    bool IsVisible() const { return bVisible; }
protected:

    FBoundingBox AABB;
    bool bVisible = true;

public:
    FBoundingBox GetBoundingBox() const {return AABB;}
};
