// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Weapons/WeaponInterface.h"

#include "WeaponBase.generated.h"

UCLASS()
class ZERO_API AWeaponBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
public:
	AWeaponBase();

	// Components
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMeshWeapon_FP = nullptr;
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMeshWeapon_TP = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* MyAudioComponent = nullptr;

	// Settings
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	FName SocketName = "GripPoint";
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	float RateOfFire = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	int8 BulletOnShoot = 1;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	float Damage = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	TSubclassOf<class AZeroProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 25.0f);

protected:
	// Base
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Variables
	UPROPERTY()
	USkeletalMeshComponent* FP_MeshForAnim = nullptr;
	UPROPERTY()
	USkeletalMeshComponent* TP_MeshForAnim = nullptr;

	FVector FireStartPoint;
	FVector FireForwardVector;

	FTimerHandle HideTimer;
	
	float FireTimer = 0.0f;
	bool WeaponFiring = false;

	// Functions protected
	UFUNCTION(Server, Reliable)
	void FireStart_OnServer();
	UFUNCTION(Server, Reliable)
	void FireStop_OnServer();

	void FireTick(float DeltaTime);
	UFUNCTION()
	void Fire();
	
	UFUNCTION(NetMulticast, Reliable)
	void AttachMeshes_Multicast(USkeletalMeshComponent* FP_Mesh, USkeletalMeshComponent* TP_Mesh);
	UFUNCTION(NetMulticast, Unreliable)
	void FireEffect_Multicast(FVector StartPoint, FVector ForwardVector);

public:
	// Functions public
	UFUNCTION(Server, Reliable)
	void AttachMeshes_OnServer(USkeletalMeshComponent* FP_Mesh, USkeletalMeshComponent* TP_Mesh);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform GetSnapPoint();
	
	// Interface
	virtual void FireRequest(bool bStatus) override;
	virtual void ReloadRequest() override;
};
