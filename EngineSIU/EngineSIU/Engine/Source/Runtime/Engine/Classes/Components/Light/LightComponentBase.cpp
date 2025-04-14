#include "LightComponentBase.h"


ULightComponentBase::ULightComponentBase()
{
}
UObject* ULightComponentBase::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    // Light 데이터 복사
    NewComponent->SetLight(this->GetLight());

    return NewComponent;
}
