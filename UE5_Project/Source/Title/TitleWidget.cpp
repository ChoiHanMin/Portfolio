// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Components/Button.h"
#include "TitlePC.h"
#include "Portfolio/PlayableGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UTitleWidget::StartGame);
	}
}

void UTitleWidget::StartGame()
{
	// Get the GameInstance
	UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OpenLevel();
	}
}
