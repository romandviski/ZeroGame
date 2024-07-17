// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WeaponInterface.generated.h"



UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};
class ZERO_API IWeaponInterface
{
	GENERATED_BODY()

public:
	virtual void FireRequest(bool bStatus);
	virtual void ReloadRequest();
};
