// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ZeroGameInstance.h"
#include "GameFramework/GameUserSettings.h"

UZeroGameInstance::UZeroGameInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{	

}

void UZeroGameInstance::Init()
{
	Super::Init();

	// Пример настройки и ограничение FPS через консольную команду
	GEngine->GameUserSettings->SetVSyncEnabled(true);
	GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	GEngine->GameUserSettings->ApplySettings(true);
	GEngine->GameUserSettings->SaveSettings();
	GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 60"));
}
