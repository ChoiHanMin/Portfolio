// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyGM.h"
#include "TimerManager.h"
#include "Lobby/LobbyGS.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ALobbyGM::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(StartTimerHandle,
		this,
		&ALobbyGM::StartTimer,
		1.0f,
		true,
		0);
}

void ALobbyGM::StartTimer()
{
	auto GS = GetGameState<ALobbyGS>();

	if (GS)
	{
		GS->LeftTime--;
		if (HasAuthority())
		{
			GS->OnRep_LeftTime();
			if (GS->LeftTime < 0)
			{
				GetWorldTimerManager().ClearTimer(StartTimerHandle);
			}
		}
	}
}

void ALobbyGM::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto GS = GetGameState<ALobbyGS>();

	if (GS)
	{
		//PlayerState °¹¼ö ¼¼±â
		GS->UserCount = GS->PlayerArray.Num();

		if (HasAuthority())
		{
			GS->OnRep_UserCount();
		}
	}
}

