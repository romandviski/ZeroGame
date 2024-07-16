// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Zero/Weapons/WeaponBase.h"

#include "InventoryComponent.generated.h"

/**
 *  Компонент инвентаря, спавнит оружие
 *  и хранит информацию о патронах
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ZERO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// класс оружия по умолчанию Установлен в блюпринте
	UPROPERTY(EditDefaultsOnly, Category = Settings)
	TSubclassOf<AWeaponBase> SpawnWeaponClass;
	
	UPROPERTY()
	USkeletalMeshComponent* PlayerFP_Mesh = nullptr;
	UPROPERTY()
	USkeletalMeshComponent* PlayerTP_Mesh = nullptr;

	UPROPERTY(Replicated)
	AWeaponBase* CurrentWeapon = nullptr;
	
public:
	UFUNCTION(Server, Reliable)
	void InitWeapon_OnServer(USkeletalMeshComponent* FP_Mesh, USkeletalMeshComponent* TP_Mesh);

	UFUNCTION(Server, Reliable)
	void SetNewWeapon_OnServer(TSubclassOf<AWeaponBase> NewWeapon);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapons)
	FORCEINLINE AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }
};
