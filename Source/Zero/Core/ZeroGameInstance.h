// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "ZeroGameInstance.generated.h"



UCLASS()
class ZERO_API UZeroGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UZeroGameInstance();

protected:
	virtual void Init() override;
};
