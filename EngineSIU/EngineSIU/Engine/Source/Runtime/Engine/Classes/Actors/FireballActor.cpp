#include "FireballActor.h"
#include "Engine/FLoaderOBJ.h"

#include "Components/Light/PointLightComponent.h"

#include "Components/ProjectileMovementComponent.h"

#include "Components/SphereComp.h"

AFireballActor::AFireballActor()
{
    FManagerOBJ::CreateStaticMesh("Contents/Sphere.obj");


    SphereComp = AddComponent<USphereComp>();
    
    SphereComp->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"Contents/Sphere.obj"));
    SetRootComponent(SphereComp);

  
    PointLightComponent = AddComponent<UPointLightComponent>();
    

    //PointLightComponent->SetAttenuationRadius(0.5f);
    //PointLightComponent->SetFalloff(2.0f);
    PointLightComponent->SetLight(FLight(FLinearColor::Red, FLinearColor::Red, 1.0f));
    
    ProjectileMovementComponent = AddComponent<UProjectileMovementComponent>();
    PointLightComponent->AttachToComponent(RootComponent);

    ProjectileMovementComponent->SetGravity(0);
    ProjectileMovementComponent->SetVelocity(FVector(100, 0, 0));
    ProjectileMovementComponent->SetInitialSpeed(100);
    ProjectileMovementComponent->SetMaxSpeed(100);
    ProjectileMovementComponent->SetLifetime(10);
}

AFireballActor::~AFireballActor()
{
}

void AFireballActor::BeginPlay()
{
}
