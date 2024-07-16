// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::InitWeapon_OnServer_Implementation(USkeletalMeshComponent* FP_Mesh, USkeletalMeshComponent* TP_Mesh)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	
	if (SpawnWeaponClass)
	{
		PlayerFP_Mesh = FP_Mesh;
		PlayerTP_Mesh = TP_Mesh;
			
		CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnWeaponClass, FTransform(FVector(0, 0, 0)));
		CurrentWeapon->SetOwner(GetOwner());
		CurrentWeapon->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform, "none");
		CurrentWeapon->AttachMeshes_OnServer(PlayerFP_Mesh, PlayerTP_Mesh);
	}
}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
}

void UInventoryComponent::SetNewWeapon_OnServer_Implementation(TSubclassOf<AWeaponBase> NewWeapon)
{
	SpawnWeaponClass = NewWeapon;
	InitWeapon_OnServer(PlayerFP_Mesh, PlayerTP_Mesh);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, CurrentWeapon);
}
