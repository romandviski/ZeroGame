// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

/**
 *  Компонент здоровья, содержит жизни игрока.
 *  Сообщает о смерти игрока через диспатчер
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZERO_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChange OnHealthChange;
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDead OnDead;
	
protected:
	UPROPERTY(Replicated)
	float Health = 100.0f;
	UPROPERTY(Replicated)
	bool bAliveStatus = true;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return Health;}
	UFUNCTION(BlueprintCallable, Category = "Health")
	FORCEINLINE bool GetAliveStatus() const { return bAliveStatus;}

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Health")
	virtual void ChangeHealthValue_OnServer(float ChangeValue);

	UFUNCTION(NetMulticast, Reliable)
	void HealthChangeEvent_Multicast(float newHealth, float value);
	
	UFUNCTION(NetMulticast, Reliable)
	void DeadEvent_Multicast();
};
