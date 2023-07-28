// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZeroCharacter.h"

#include "../Weapons/Projectiles/ZeroProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InventoryComponent.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Zero/Core/ZeroPlayerState.h"

//////////////////////////////////////////////////////////////////////////
// AZeroCharacter

AZeroCharacter::AZeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	// Character doesnt have a rifle at start
	bHasRifle = true;
	
	// Мои настройки
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetIsReplicated(true);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh_FP"));
	Mesh_FP->SetupAttachment(FirstPersonCameraComponent);
	Mesh_FP->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	Mesh_FP->bCastDynamicShadow = false;
	Mesh_FP->CastShadow = false;
	Mesh_FP->SetOnlyOwnerSee(true);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &AZeroCharacter::CharDead);
	}
}

void AZeroCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (HasAuthority())
	{
		if (InventoryComponent)
		{
			InventoryComponent->InitWeapon_OnServer(Mesh_FP, GetMesh());
		}
	}
}

void AZeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetController() && (IsLocallyControlled() || HasAuthority()))
	{
		CameraLocation_Rep = FirstPersonCameraComponent->GetComponentLocation();
		ControlRotation_Rep = GetController()->GetControlRotation();
		CameraForwardVector_Rep = UKismetMathLibrary::GetForwardVector(ControlRotation_Rep);
	}
}

void AZeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AZeroCharacter::InputMove);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AZeroCharacter::InputLook);
		
		// Weapon
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AZeroCharacter::InputAttackPressed); // Started
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AZeroCharacter::InputAttackReleased);

		// Exit
		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Started, this, &AZeroCharacter::InputExitPressed);
	}
}

void AZeroCharacter::InputMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AZeroCharacter::InputLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AZeroCharacter::InputAttackPressed(const FInputActionValue& Value)
{
	//bool bStatus = Value.Get<bool>(); // не прокатило чёт =(

	FireStatus(true);
}

void AZeroCharacter::InputAttackReleased(const FInputActionValue& Value)
{
	FireStatus(false);
}

void AZeroCharacter::InputExitPressed(const FInputActionValue& Value)
{
	UGameplayStatics::OpenLevel(GetWorld(), "MENU", true, "");
}

float AZeroCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		if (HealthComponent && HealthComponent->GetIsAlive())
		{
			HealthComponent->ChangeHealthValue_OnServer(-DamageAmount);

			bool bSelf = DamageCauser == this;
			int8 Score = bSelf ? -50 : 100;
			if (!HealthComponent->GetIsAlive())
			{
				auto KillerController = Cast<APlayerController>(EventInstigator);
				if (KillerController != nullptr)
				{
					auto KillerState = KillerController->GetPlayerState<AZeroPlayerState>();
					if (KillerState != nullptr)
					{
						KillerState->ChangeGameScore_OnServer(Score);
					}
				}
			}
		}
	}

	return ActualDamage;
}

void AZeroCharacter::CharDead()
{
	if (HasAuthority())
	{
	/* // Анимация смерти, так себе
		float TimeAnim = FMath::FRandRange(0.0f, 0.0f);
		int32 rnd = FMath::RandHelper(DeadsAnim.Num());
		if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
		{
			TimeAnim = DeadsAnim[rnd]->GetPlayLength();
			//GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
			PlayAnim_Multicast(DeadsAnim[rnd]);
		}
	*/
		FireStatus(false);
		
		if (GetController())
		{
			GetController()->UnPossess();
		}

		//GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &AZeroCharacter::EnableRagdoll_Multicast, TimeAnim, false);
		EnableRagdoll_Multicast();
		SetLifeSpan(60.0f);
	}
}

void AZeroCharacter::EnableRagdoll_Multicast_Implementation()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
	}	
}

void AZeroCharacter::FireStatus_Implementation(bool bStatus)
{
	if (HasAuthority() && HealthComponent->GetIsAlive() && InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->FireRequest(bStatus);
	}
	else if (HasAuthority() && InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->FireRequest(bStatus);
	}
}

FVector AZeroCharacter::GetFireLocation()
{
	return CameraLocation_Rep;
}

FVector AZeroCharacter::GetFireVector()
{
	return CameraForwardVector_Rep;
}

void AZeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZeroCharacter, Mesh_FP);
	DOREPLIFETIME(AZeroCharacter, CameraLocation_Rep);
	DOREPLIFETIME(AZeroCharacter, CameraForwardVector_Rep);
	DOREPLIFETIME(AZeroCharacter, ControlRotation_Rep);
}
