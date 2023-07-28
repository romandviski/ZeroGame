// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Zero/Weapons/WeaponBase.h"

#include "WorldWeapon.generated.h"

UCLASS()
class ZERO_API AWorldWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWorldWeapon();

	UPROPERTY(EditDefaultsOnly, Category = Components)
	class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class UBoxComponent* CollisionBox = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class URotatingMovementComponent* MovementComponent = nullptr;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void SetNewWeapon_OnServer(AActor* OtherActor);

	FTimerHandle ReloadTimer;
	bool bActive = true;

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
