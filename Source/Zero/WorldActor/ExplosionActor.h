// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ExplosionActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USphereComponent;
class URadialForceComponent;

UCLASS()
class ZERO_API AExplosionActor : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* RadialForce = nullptr;

	FTimerHandle ReloadTimer;
	bool bActive = true;
	
public:
	// Sets default values for this actor's properties
	AExplosionActor();

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(Server, Reliable)
	void ExplosionEffect_OnServer();
	UFUNCTION(NetMulticast, Unreliable)
	void ExplosionEffect_Multicast();
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
