// Fill out your copyright notice in the Description page of Project Settings.


#include "ZeroPlayerState.h"

#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AZeroPlayerState::AZeroPlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AZeroPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZeroPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZeroPlayerState::ChangeGameScore_OnServer_Implementation(int32 AddedScore)
{
	//GameScore = GameScore + UKismetMathLibrary::Abs(AddedScore);
	GameScore = GameScore + AddedScore;
}

void AZeroPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZeroPlayerState, GameScore);
}

