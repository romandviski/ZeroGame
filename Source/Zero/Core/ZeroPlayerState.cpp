// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeroPlayerState.h"
#include "Net/UnrealNetwork.h"



AZeroPlayerState::AZeroPlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AZeroPlayerState::ChangePlayerScore_OnServer_Implementation(int32 AddedScore)
{
	PlayerScore += AddedScore;
	ChangePlayerScore_Multicast(PlayerScore);
}

void AZeroPlayerState::ChangePlayerScore_Multicast_Implementation(int32 CurrentScore)
{
	ScoreChanged.Broadcast(CurrentScore);
}

void AZeroPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZeroPlayerState, PlayerScore);
}
