// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Ingame/InGamePlayerController.h"
#include "Title/TitlePC.h"
#include "SaveGameDatas.h"
#include "Components/AudioComponent.h"

void UPlayableGameInstance::Init()
{
	Super::Init();
	CurrentLoadMap = ELoadedMap::Title;
	FadeOutDelegate.BindUObject(this, &UPlayableGameInstance::GameLoadComplete);
}

void UPlayableGameInstance::Shutdown()
{
	Super::Shutdown();
	BGMFadeOut();
}

void UPlayableGameInstance::OnStart()
{
	Super::OnStart();
	BGM_Volume_Delegate.BindUObject(this, &UPlayableGameInstance::SetBGMVolume);

	if (USaveGameDatas* LoadedGame = Cast<USaveGameDatas>(UGameplayStatics::LoadGameFromSlot(TEXT("SavedDatas"), 0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("LOADED: %f"), LoadedGame->BGMVolume);
	}
}

void UPlayableGameInstance::BGMFadeIn(float StartTime, USoundBase* BGM)
{
	float Volume = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("Sound: %s"), *BGM->GetName());
	USaveGameDatas* LoadedGame = Cast<USaveGameDatas>(UGameplayStatics::LoadGameFromSlot(TEXT("SavedDatas"), 0));
	if (LoadedGame != nullptr)
	{
		Volume = LoadedGame->BGMVolume;
		UE_LOG(LogTemp, Warning, TEXT("LOADED: %f"), LoadedGame->BGMVolume);
	}
	else
	{
		Volume = 1.0f;
	}
	
	UWorld* World = GetWorld();
	BGMComponent = UGameplayStatics::CreateSound2D(World, BGM, Volume, 1.0f, 0.0f, nullptr, true);
	BGMComponent->FadeIn(BGMFadeDuration, Volume, StartTime, EAudioFaderCurve::Linear);
	
	BGMComponent->Play();
}

void UPlayableGameInstance::BGMFadeOut()
{
	if (BGMComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("BGM FadeOut"));
		BGMComponent->FadeOut(BGMFadeDuration, 0, EAudioFaderCurve::Linear);
	}
}

void UPlayableGameInstance::SetBGMVolume(float Volume)
{
	if (BGMComponent)
	{
		BGMComponent->SetVolumeMultiplier(Volume);
	}
}

void UPlayableGameInstance::OpenLevel()
{
	BGMFadeOut();

	UE_LOG(LogTemp, Warning, TEXT("OpenLevel: %s"), *UEnum::GetValueAsString(CurrentLoadMap));
	if (CurrentLoadMap == ELoadedMap::InGame)
	{
		CurrentLoadMap = ELoadedMap::Title;
		UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/Maps/TitleMap"));
	}
	else if (CurrentLoadMap == ELoadedMap::Title)
	{
		CurrentLoadMap = ELoadedMap::InGame;
		UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/Maps/GameMap"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentLoadMap is NONE"));
	}
}

void UPlayableGameInstance::GameLoadComplete()
{
	bIsCallLoad = false;
	if (CurrentLoadMap == ELoadedMap::InGame)
	{
		AInGamePlayerController* PlayerController = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PlayerController)
		{
			bIsCallLoad = true;
			UE_LOG(LogTemp, Warning, TEXT("PC PC PC PC PC PC PC PC PC PC"));
			PlayerController->LoadingEnd();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GameLoadComplete"));
}
