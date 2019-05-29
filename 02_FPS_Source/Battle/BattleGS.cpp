// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGS.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" //GetWorld()
#include "Battle/BattlePC.h"
#include "Battle/BattleWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Battle/ConclusionWidgetBase.h"
#include "Battle/BattleGM.h"
#include "Public/TimerManager.h" //GetTimerManger()

ABattleGS::ABattleGS()
{
	SetReplicates(true);
}

void ABattleGS::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleGS, AliveCount);
	DOREPLIFETIME(ABattleGS, TotalCount);
	DOREPLIFETIME(ABattleGS, InfoMessage);
	DOREPLIFETIME(ABattleGS, PublicMessage);
}


void ABattleGS::OnRep_AliveCount()
{
	//UI에 값을 전달
	ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		UE_LOG(LogClass, Warning, TEXT("OnRep_AliveCount %d"), AliveCount);

		if (PC->BattleWidget && PC->BattleWidget->AliveCount)
		{
			UE_LOG(LogClass, Warning, TEXT("Alive Count %d"), AliveCount);
			FText Message = FText::FromString(FString::Printf(TEXT("%d 생존"), AliveCount));
			PC->BattleWidget->AliveCount->SetText(Message);
		}
	}
}

void ABattleGS::OnRep_TotalCount()
{
}

void ABattleGS::OnRep_InfoMessage()
{
	ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		if (PC->BattleWidget && PC->BattleWidget->Info)
		{
			PC->BattleWidget->AddInfo(InfoMessage);
		}
	}
}

void ABattleGS::OnRep_PublicMessage()
{
	//UI에 값을 전달
	ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		if (PC->ConclusionWidget && PC->ConclusionWidget->PublicMessage)
		{
			UE_LOG(LogClass, Warning, TEXT("OnRep_PublicMessage %s"), *PublicMessage);

			PC->ConclusionWidget->PublicMessage->SetText(FText::FromString(PublicMessage));
		}
	}
}
