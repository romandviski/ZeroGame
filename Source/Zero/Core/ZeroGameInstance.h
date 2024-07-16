// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"

#include "ZeroGameInstance.generated.h"



UCLASS()
class ZERO_API UZeroGameInstance : public UAdvancedFriendsGameInstance // Наследуюсь от GameInstance из плагина AdvancedSessions
{
	GENERATED_BODY()

public:
	UZeroGameInstance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Init() override;
};
