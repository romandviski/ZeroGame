// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ExplosionActor.generated.h"

UCLASS()
class ZERO_API AExplosionActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AExplosionActor();

	UPROPERTY(EditDefaultsOnly, Category = Components)
	class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class USphereComponent* CollisionSphere = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components)
	class URadialForceComponent* RadialForce = nullptr;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Server, Reliable)
	void ExplosionEffect_OnServer();
	
	UFUNCTION(NetMulticast, Unreliable)
	void ExplosionEffect_Multicast();

	FTimerHandle ReloadTimer;
	bool bActive = true;

	UFUNCTION(NetMulticast, Unreliable)
	void Reload_Multicast();
	
public:
	UPROPERTY(EditInstanceOnly, Category = "Settings")
	float Radius = 250.0f;
	UPROPERTY(EditInstanceOnly, Category = "Settings")
	float Damage = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	UParticleSystem* FxFire = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	USoundBase* SoundFire = nullptr;
};
