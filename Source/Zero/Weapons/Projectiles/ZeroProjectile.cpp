// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZeroProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WorldActor/ExplosionActor.h"
#include "Characters/ZeroCharacter.h"



AZeroProjectile::AZeroProjectile() 
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// сфера в качестве простого представления столкновений
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->OnComponentHit.AddDynamic(this, &AZeroProjectile::OnHit); // уведомление, когда этот компонент столкнется с чем-то блокирующим

	RootComponent = CollisionComp;

	// компонент ProjectileMovementComponent для управления движением этого снаряда
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// По умолчанию умирает через несколько секунд
	InitialLifeSpan = 3.0f;
}

void AZeroProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AZeroCharacter>(OtherActor) || Cast<AExplosionActor>(OtherActor))
	{
		const auto OwnerPlayer = Cast<AZeroCharacter>(GetOwner());
		const auto PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());
		
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Hit.TraceStart,Hit, PlayerController,GetOwner(),NULL);
		Destroy();
	}
	
	if ((OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 50.0f, GetActorLocation());
		Destroy();
	}
}
