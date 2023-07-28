// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "ZeroCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AZeroCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh_FP;
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	/** Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ExitAction;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	/** input */
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputAttackPressed(const FInputActionValue& Value);
	void InputAttackReleased(const FInputActionValue& Value);
	void InputExitPressed(const FInputActionValue& Value);
	
	UFUNCTION()
	void CharDead();
	UFUNCTION(NetMulticast, Reliable)
	void EnableRagdoll_Multicast();
	
	// Variables
	UPROPERTY(Replicated)
	FVector CameraLocation_Rep;
	
public:
	AZeroCharacter();
	
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	FTimerHandle TimerHandle_RagDollTimer;
	
	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	bool bHasRifle;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Animation)
	FRotator ControlRotation_Rep;
	UPROPERTY(Replicated)
	FVector CameraForwardVector_Rep;
	
	FVector GetFireLocation();
	FVector GetFireVector();
	
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh_FP; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(Server, Reliable)
	void FireStatus(bool bStatus);
	
};

