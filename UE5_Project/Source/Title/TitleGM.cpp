// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGM.h"
#include "Portfolio/PlayableGameInstance.h"

void ATitleGM::BeginPlay()
{
	Super::BeginPlay();

	UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->BGMFadeIn(0.0f,GameInstance->TitleBGMSound);
	}
}
