// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZeroCharacter.h"

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



AZeroCharacter::AZeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
	
	// Свои настройки
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetIsReplicated(true);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// компонент сетки, который будет использоваться при виде от «1-го лица»
	Mesh_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh_FP"));
	Mesh_FP->SetupAttachment(FirstPersonCamera);
	Mesh_FP->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	Mesh_FP->bCastDynamicShadow = false;
	Mesh_FP->CastShadow = false;
	Mesh_FP->SetOnlyOwnerSee(true);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &AZeroCharacter::CharDead);
		HealthComponent->OnHealthChange.AddDynamic(this, &AZeroCharacter::HealthChanged);
	}
}

void AZeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (HasAuthority() && InventoryComponent)
	{
		InventoryComponent->InitWeapon_OnServer(Mesh_FP, GetMesh());
	}
}

void AZeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetController() && (IsLocallyControlled() || HasAuthority()))
	{
		CameraLocation_Rep = FirstPersonCamera->GetComponentLocation();
		ControlRotation_Rep = GetController()->GetControlRotation();
		CameraForwardVector_Rep = UKismetMathLibrary::GetForwardVector(ControlRotation_Rep);
	}
}

void AZeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AZeroCharacter::InputMove);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AZeroCharacter::InputLook);
		
		// Weapon
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AZeroCharacter::InputAttackPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AZeroCharacter::InputAttackReleased);

		// Exit
		EnhancedInputComponent->BindAction(ExitAction, ETriggerEvent::Started, this, &AZeroCharacter::InputExitPressed);
	}
}

void AZeroCharacter::InputMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
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

	if (Controller)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AZeroCharacter::InputAttackPressed(const FInputActionValue& Value)
{
	ChangeFireStatus_OnServer(true);
}

void AZeroCharacter::InputAttackReleased(const FInputActionValue& Value)
{
	ChangeFireStatus_OnServer(false);
}

void AZeroCharacter::InputExitPressed(const FInputActionValue& Value)
{
	UGameplayStatics::OpenLevel(GetWorld(), "MENU", true, "");
}

float AZeroCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (HasAuthority() && HealthComponent)
	{
		if (HealthComponent->GetAliveStatus())
		{
			HealthComponent->ChangeHealthValue_OnServer(-DamageAmount);

			if (!HealthComponent->GetAliveStatus())
			{
				if (const auto KillerController = Cast<APlayerController>(EventInstigator))
				{
					if (const auto KillerState = KillerController->GetPlayerState<AZeroPlayerState>())
					{
						const int8 Score = DamageCauser == this ? -50 : 100; // Для начисление очков за убийство (не красиво)
						KillerState->ChangePlayerScore_OnServer(Score);
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
		ChangeFireStatus_OnServer(false);
		
		if (GetController())
		{
			GetController()->UnPossess();
		}
		
		EnableRagdoll_Multicast();
	}
}

void AZeroCharacter::HealthChanged(float CurrentHealth, float Damage)
{
	CharacterHealthChanged.Broadcast(CurrentHealth);
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
	
	SetLifeSpan(60.0f);
}

void AZeroCharacter::ChangeFireStatus_OnServer_Implementation(const bool bNewStatus)
{
	if (HasAuthority() && HealthComponent && HealthComponent->GetAliveStatus() && InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->FireRequest(bNewStatus);
	}
	else if (HasAuthority() && InventoryComponent && InventoryComponent->GetCurrentWeapon())
	{
		InventoryComponent->GetCurrentWeapon()->FireRequest(bNewStatus);
	}
}

void AZeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZeroCharacter, Mesh_FP);
	DOREPLIFETIME(AZeroCharacter, CameraLocation_Rep);
	DOREPLIFETIME(AZeroCharacter, CameraForwardVector_Rep);
	DOREPLIFETIME(AZeroCharacter, ControlRotation_Rep);
}
