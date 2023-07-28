// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionActor.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AExplosionActor::AExplosionActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("My Scene"));
	RootComponent = SceneComponent;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("My Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetRelativeLocation(FVector(0, 0, 50));

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("My Collision Sphere"));
	CollisionSphere->SetupAttachment(StaticMesh);
	CollisionSphere->SetSphereRadius(Radius);
	CollisionSphere->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionSphere->CanCharacterStepUpOn = ECB_No;
	CollisionSphere->SetCollisionResponseToChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("My Radial Force"));
	RadialForce->SetupAttachment(StaticMesh);
	RadialForce->Radius = Radius;

	// в блюпринте
	//RadialForce->bImpulseVelChange(true);
	//RadialForce->ImpulseStrength(Damage * 10);
}

// Called when the game starts or when spawned
void AExplosionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AExplosionActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (bActive)
	{
		TArray<AActor*> OverlappingActors;
		CollisionSphere->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

		if (OverlappingActors.Num() > 0)
		{
			for (AActor* CurrentActor : OverlappingActors) // for each
			{
				UGameplayStatics::ApplyDamage(CurrentActor, Damage, EventInstigator, DamageCauser, NULL);
			}
		}
		if (HasAuthority())
		{
			ExplosionEffect_OnServer();
		}
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AExplosionActor::ExplosionEffect_OnServer_Implementation()
{
	bActive = false;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AExplosionActor::Reload_Multicast, 10.0f, false);
	
	ExplosionEffect_Multicast();
}

void AExplosionActor::ExplosionEffect_Multicast_Implementation()
{
	if (StaticMesh)
	{
		StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
		StaticMesh->SetHiddenInGame(true);
	}
	
	if (FxFire)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FxFire, GetTransform());
	}
	
	if (SoundFire)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundFire, GetActorLocation());
	}

	if (RadialForce)
	{
		RadialForce->FireImpulse();
	}
}

void AExplosionActor::Reload_Multicast_Implementation()
{
	bActive = true;
	
	if (StaticMesh)
	{
		StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
		StaticMesh->SetHiddenInGame(false);
	}
}
