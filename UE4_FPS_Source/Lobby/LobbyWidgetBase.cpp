// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Lobby/LobbyPC.h"
#include "Kismet/GameplayStatics.h"
#include "BDGameInstance.h"

void ULobbyWidgetBase::NativeConstruct()
{
	PublicMessage = Cast<UTextBlock>(GetWidgetFromName(TEXT("PublicMessage")));
	Alive = Cast<UTextBlock>(GetWidgetFromName(TEXT("Alive")));
	ChattingBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChattingBox")));

	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::StartGame);
	}

	ChattingInput = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ChattingInput")));
	if (ChattingInput)
	{
		ChattingInput->OnTextCommitted.AddDynamic(this, &ULobbyWidgetBase::OnTextCommited);
	}
}

void ULobbyWidgetBase::StartGame()
{
	ALobbyPC* PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->StartGame();
	}
}


void ULobbyWidgetBase::OnTextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	ALobbyPC* PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		if (CommitMethod == ETextCommit::OnEnter)
		{
			if (Text.IsEmpty())
			{
				//채팅 끝
			}
			else
			{
				//Host -> PC 메세지 전달, 이름 : 메세지
				auto GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				if (GI)
				{
					FString Message;
					Message = FString::Printf(TEXT("%s : %s"), *GI->UserID, *Text.ToString());
					PC->C2S_SendChatMessage(FText::FromString(*Message));

					ChattingInput->SetText(FText::FromString(TEXT("")));
				}
			}
		}
		else if (CommitMethod == ETextCommit::OnCleared)
		{
			if (PC)
			{
				ChattingInput->SetUserFocus(PC);
			}
		}
	}
}

void ULobbyWidgetBase::HideButton()
{
	StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
}

void ULobbyWidgetBase::AddChatMessage(const FText& Message)
{
	if (ChattingBox)
	{
		UTextBlock* NewMessage = NewObject<UTextBlock>(ChattingBox);
		if (NewMessage)
		{
			NewMessage->SetText(Message);
			ChattingBox->AddChild(NewMessage);
			ChattingBox->ScrollToEnd();
		}
	}
}
