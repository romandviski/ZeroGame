// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Characters/ZeroCharacter.h"
#include "../Weapons/Projectiles/ZeroProjectile.h"
#include "Components/AudioComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("My Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon For FP"));
	SkeletalMeshWeapon_FP->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon_FP->SetCollisionProfileName(TEXT("NoCollision"));
	//SkeletalMeshWeapon_FP->SetupAttachment(GetRootComponent());
	SkeletalMeshWeapon_FP->bCastDynamicShadow = false;
	SkeletalMeshWeapon_FP->CastShadow = false;
	SkeletalMeshWeapon_FP->SetOnlyOwnerSee(true);
	SkeletalMeshWeapon_FP->SetIsReplicated(true);

	SkeletalMeshWeapon_TP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon For TP"));
	SkeletalMeshWeapon_TP->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon_TP->SetCollisionProfileName(TEXT("NoCollision"));
	//SkeletalMeshWeapon_TP->SetupAttachment(GetRootComponent());
	SkeletalMeshWeapon_TP->SetOwnerNoSee(true);
	SkeletalMeshWeapon_TP->SetIsReplicated(true);

	MyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("My Audio Component"));
	MyAudioComponent->SetupAttachment(GetRootComponent());
	MyAudioComponent->SetAutoActivate(false);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FireTick(DeltaTime);
	}
}

void AWeaponBase::AttachMeshes_OnServer_Implementation(USkeletalMeshComponent* FP_Mesh, USkeletalMeshComponent* TP_Mesh)
{
	AttachMeshes_Multicast(FP_Mesh, TP_Mesh);
}

void AWeaponBase::AttachMeshes_Multicast_Implementation(USkeletalMeshComponent* FP_Mesh, USkeletalMeshComponent* TP_Mesh)
{
	FP_MeshForAnim = FP_Mesh;
	TP_MeshForAnim = TP_Mesh;

	if (SkeletalMeshWeapon_FP)
	{
		SkeletalMeshWeapon_FP->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		SkeletalMeshWeapon_FP->AttachToComponent(FP_MeshForAnim, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	}
	if (SkeletalMeshWeapon_TP)
	{
		SkeletalMeshWeapon_TP->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform); // без детача крашит при завершении игры О_о
		SkeletalMeshWeapon_TP->AttachToComponent(TP_MeshForAnim, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	}
}

void AWeaponBase::FireStart_OnServer_Implementation()
{
	WeaponFiring = true;
}

void AWeaponBase::FireStop_OnServer_Implementation()
{
	WeaponFiring = false;
}

FTransform AWeaponBase::GetSnapPoint()
{
	FTransform Result = FTransform();

	if (SkeletalMeshWeapon_TP)
	{
		Result = SkeletalMeshWeapon_TP->GetSocketTransform("LeftHandPlacement",RTS_World);
	}
	
	return Result;
}

void AWeaponBase::FireRequest(bool bStatus)
{
	if (bStatus)
	{
		FireStart_OnServer();
	}
	else
	{
		FireStop_OnServer();
	}
}

void AWeaponBase::ReloadRequest()
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Reload started"));
}

void AWeaponBase::FireTick(float DeltaTime)
{
	if (HasAuthority())
	{
		if (WeaponFiring && FireTimer < 0.0f)
		{
			Fire();
		}
		else
			FireTimer -= DeltaTime;
	}
}

void AWeaponBase::Fire()
{
	auto OwnerPlayer = Cast<AZeroCharacter>(GetOwner());
	auto PlayerController = Cast<APlayerController>(OwnerPlayer->GetController());

	FireTimer = RateOfFire;
	FireStartPoint = OwnerPlayer->GetFireLocation();
	FireForwardVector = OwnerPlayer->GetFireVector();
	
	for (int8 i = 0; i < BulletOnShoot; i++) // Shotgun... maybe later...
	{
		FHitResult Hit;
		TArray<AActor*> Actors;
		Actors.Add(GetOwner());
		EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::ForDuration;
		
		if (ProjectileClass)
		{
			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				/* получение направления через PlayerCameraManager (потом подумаю, чёт сразу не завелась)
				const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
				const FVector SpawnLocation = SkeletalMeshWeapon_FP->GetComponentLocation() + SpawnRotation.RotateVector(MuzzleOffset);
				*/
				const FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromX(FireForwardVector);
				const FVector SpawnLocation = SkeletalMeshWeapon_FP->GetComponentLocation() + SpawnRotation.RotateVector(MuzzleOffset);

				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				
				auto Projectile = World->SpawnActor<AZeroProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				if (Projectile)
				{
					Projectile->Damage = this->Damage;
					Projectile->SetOwner(OwnerPlayer);
				}
				
				FireEffect_Multicast(FVector(ForceInitToZero), FVector(ForceInitToZero));
			}
		}
		else
		{
			FireEffect_Multicast(FireStartPoint, FireForwardVector);
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), FireStartPoint,FireStartPoint + FireForwardVector * 50000, ETraceTypeQuery::TraceTypeQuery2,
									false, Actors, DebugTrace, Hit, true, FLinearColor::Red,FLinearColor::Green, 0.0f);

			if (Hit.GetComponent() && Hit.GetComponent()->IsSimulatingPhysics())
			{
				// для физики трупа нужен мультикаст
				Hit.GetComponent()->AddImpulseAtLocation(FireForwardVector * 1000000,Hit.Location, Hit.BoneName);
			}
			else
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, Hit.TraceStart, Hit, PlayerController,OwnerPlayer,NULL);	
		}
	}
}

void AWeaponBase::FireEffect_Multicast_Implementation(FVector StartPoint, FVector ForwardVector)
{
	MyAudioComponent->Play();
	DrawDebugLine(GetWorld(), StartPoint, StartPoint + ForwardVector * 10000, FColor::Yellow, false, 0.5f, (uint8)'\000', 0.5f);
}
