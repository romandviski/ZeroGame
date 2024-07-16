// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"

#include "Net/UnrealNetwork.h"



UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::ChangeHealthValue_OnServer_Implementation(float ChangeValue)
{
	if (bAliveStatus)
	{
		Health += ChangeValue;

		// Логирование https://www.chrismccole.com/blog/logging-in-ue4-cpp
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 999.f, FColor::Red, FString::Printf(TEXT("Health %f"), ChangeValue));

		if (Health > 100.0f)
		{
			Health = 100.0f;
		}
		
		HealthChangeEvent_Multicast(Health, ChangeValue);

		if (Health <= 0.0f)
		{
			bAliveStatus = false;
			DeadEvent_Multicast();
		}
	}
}

void UHealthComponent::HealthChangeEvent_Multicast_Implementation(float newHealth, float value)
{
	OnHealthChange.Broadcast(newHealth, value);
}

void UHealthComponent::DeadEvent_Multicast_Implementation()
{
	OnDead.Broadcast();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, bAliveStatus);
}
