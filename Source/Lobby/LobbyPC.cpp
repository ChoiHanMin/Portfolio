// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyPC.h"
#include "Lobby/LobbyWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameMode.h"
#include "TimerManager.h"
#include "Components/EditableTextBox.h"
#include "Lobby/LobbyGS.h"

void ALobbyPC::BeginPlay()
{
	SetInputMode(FInputModeGameAndUI());

	S2C_SetupWidget();
}

void ALobbyPC::Tick(float DeltaSeconds)
{
	 auto GS = Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetWorld()));
	 if (GS)
	 {
		 UE_LOG(LogClass, Warning, TEXT("LeftTime %d"), GS->LeftTime);
	 }
}

void ALobbyPC::S2C_SetupWidget_Implementation()
{
	FStringClassReference LobbyWidgetClassRef(TEXT("WidgetBlueprint'/Game/Blueprints/UI/Lobby/LobbyWidget.LobbyWidget_C'"));
	if (UClass* MyWidgetClass = LobbyWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		LobbyWidget = Cast<ULobbyWidgetBase>(CreateWidget<UUserWidget>(this, MyWidgetClass));
		LobbyWidget->AddToViewport();

		if (!HasAuthority())
		{
			LobbyWidget->HideButton();
		}
		else
		{
			//호스트 혼자 접속하면 숫자 업데이트
			auto GS = Cast<ALobbyGS>(UGameplayStatics::GetGameState(GetWorld()));
			if (GS)
			{
				GS->UserCount = 1;
				GS->OnRep_UserCount();
			}
		}

		LobbyWidget->ChattingInput->SetUserFocus(this);
	}

	FStringClassReference LoadingWidgetClassRef(TEXT("WidgetBlueprint'/Game/Blueprints/UI/Loading/LoadingWidget.LoadingWidget_C'"));
	if (UClass* MyWidgetClass = LoadingWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		LoadingWidget = Cast<UUserWidget>(CreateWidget<UUserWidget>(this, MyWidgetClass));
		LoadingWidget->AddToViewport();
		LoadingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ALobbyPC::StartGame()
{
	if (HasAuthority())
	{
		for (auto i = GetWorld()->GetPlayerControllerIterator(); i; ++i)
		{
			ALobbyPC* PC = Cast<ALobbyPC>(*i);
			if (PC)
			{
				PC->S2C_ViewLoadingScreen();
			}
		}
	}

	GetWorldTimerManager().SetTimer(StartGameTimer, this, &ALobbyPC::TimerStartGame, 1.0f);
}

void ALobbyPC::TimerStartGame()
{
	GetWorld()->ServerTravel(TEXT("Battle"));
}

void ALobbyPC::S2C_ViewLoadingScreen_Implementation()
{
	if (LoadingWidget)
	{
		LoadingWidget->SetVisibility(ESlateVisibility::Visible);
	}
}


bool ALobbyPC::C2S_SendChatMessage_Validate(const FText & Message)
{
	return true;
}

void ALobbyPC::C2S_SendChatMessage_Implementation(const FText& Message)
{
	//모든 PC에 메세지 전달
	for (auto i = GetWorld()->GetPlayerControllerIterator(); i; ++i)
	{
		ALobbyPC* PC = Cast<ALobbyPC>(*i);
		if (PC)
		{
			PC->S2C_AddChatMessage(Message);
		}
	}
}

void ALobbyPC::S2C_AddChatMessage_Implementation(const FText & Message)
{
	if (LobbyWidget)
	{
		LobbyWidget->AddChatMessage(Message);
	}
}
