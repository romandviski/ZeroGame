// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "ZeroCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterHealthChanged, float, CurrentHealth);

class USkeletalMeshComponent;
class UCameraComponent;
class UInventoryComponent;
class UHealthComponent;
class UInputMappingContext;
class UInputAction;

/**
 *  Персонаж игрока
 */
UCLASS(config=Game)
class AZeroCharacter : public ACharacter
{
	GENERATED_BODY()
	
private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category = Components)
	USkeletalMeshComponent* Mesh_FP;
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UHealthComponent* HealthComponent;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	/** Input Action */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ExitAction;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	/** Input functions */
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputAttackPressed(const FInputActionValue& Value);
	void InputAttackReleased(const FInputActionValue& Value);
	void InputExitPressed(const FInputActionValue& Value);
	
	UFUNCTION()
	void CharDead();
	UFUNCTION()
	void HealthChanged(float CurrentHealth, float Damage);
	
	UFUNCTION(NetMulticast, Reliable)
	void EnableRagdoll_Multicast();
	
	// Variables
	UPROPERTY(Replicated)
	FVector CameraLocation_Rep;
	UPROPERTY(Replicated)
	FRotator ControlRotation_Rep;
	UPROPERTY(Replicated)
	FVector CameraForwardVector_Rep;
	
	bool bHasRifle = true;
	
	FTimerHandle RagdollTimer;
	
public:
	AZeroCharacter();

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FCharacterHealthChanged CharacterHealthChanged;
	
	// Геттеры
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool GetWeaponStatus() const { return bHasRifle; }
	FORCEINLINE FVector GetFireLocation() const { return CameraLocation_Rep; }
	FORCEINLINE FVector GetFireVector() const { return CameraForwardVector_Rep; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE FRotator GetRepControlRotation() const { return ControlRotation_Rep; }
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh_FP; }
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCamera; }

	UFUNCTION(Server, Reliable)
	void ChangeFireStatus_OnServer(const bool bNewStatus);
};
