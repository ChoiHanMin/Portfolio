// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGS.h"
#include "UnrealNetwork.h"
#include "Lobby/LobbyPC.h"
#include "Lobby/LobbyWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

void ALobbyGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGS, LeftTime);
	DOREPLIFETIME(ALobbyGS, UserCount);

}


void ALobbyGS::OnRep_LeftTime()
{
	auto PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		if (LeftTime < 0)
		{
			//배틀맵 이동
			if (HasAuthority())
			{
				PC->StartGame();
			}
		}
		else
		{
			//UI 업데이트
			FString Temp = FString::Printf(TEXT("%d초 후에 게임이 시작 됩니다."), LeftTime);
			PC->LobbyWidget->PublicMessage->SetText(FText::FromString(Temp));
		}
	}
}

void ALobbyGS::OnRep_UserCount()
{
	auto PC = Cast<ALobbyPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		//UI 업데이트
		if (PC->LobbyWidget)
		{
			FString Temp = FString::Printf(TEXT("%d  합류"), UserCount);
			PC->LobbyWidget->Alive->SetText(FText::FromString(Temp));
		}
	}
}
