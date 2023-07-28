// Fill out your copyright notice in the Description page of Project Settings.


#include "../Core/ZeroGameInstance.h"
#include "GameFramework/GameUserSettings.h"

UZeroGameInstance::UZeroGameInstance() // конструктор
{	

}

void UZeroGameInstance::Init()
{
	Super::Init();
	
	GEngine->GameUserSettings->SetVSyncEnabled(true);
	GEngine->GameUserSettings->ApplySettings(true);
	GEngine->GameUserSettings->SaveSettings();
	GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 60"));
	
}
