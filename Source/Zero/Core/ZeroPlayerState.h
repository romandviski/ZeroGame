// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerState.h"

#include "ZeroPlayerState.generated.h"

UCLASS()
class ZERO_API AZeroPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AZeroPlayerState();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	int32 GameScore = 0;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentGameScore(){return GameScore;}

	UFUNCTION(Server, Reliable)
	void ChangeGameScore_OnServer(int32 AddedScore);
};
