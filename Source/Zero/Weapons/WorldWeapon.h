// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponBase.h"

#include "WorldWeapon.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class URotatingMovementComponent;

UCLASS()
class ZERO_API AWorldWeapon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionBox = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* MovementComponent = nullptr;

	FTimerHandle ReloadTimer;
	bool bActive = true;
	
public:
	AWorldWeapon();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void SetNewWeapon_OnServer(AActor* OtherActor);
	UFUNCTION(NetMulticast, Unreliable)
	void Reload_Multicast();
	UFUNCTION(NetMulticast, Unreliable)
	void Hide_Multicast();
	
public:
	UPROPERTY(EditInstanceOnly)
	TSubclassOf<AWeaponBase> SpawnWeaponClass;

	// переопределение нотифаев
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
