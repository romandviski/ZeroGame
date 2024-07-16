// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"

#include "ZeroPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChanged, int32, CurrentScore);

UCLASS()
class ZERO_API AZeroPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AZeroPlayerState();

protected:
	UPROPERTY(Replicated)
	int32 PlayerScore = 0;
	
public:
	UPROPERTY(BlueprintAssignable)
	FScoreChanged ScoreChanged;
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetCurrentScore() const { return PlayerScore; }

	UFUNCTION(Server, Reliable)
	void ChangePlayerScore_OnServer(int32 AddedScore);

	UFUNCTION(NetMulticast, Reliable)
	void ChangePlayerScore_Multicast(int32 CurrentScore);
};
