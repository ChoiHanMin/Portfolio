// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleGM.h"
#include "Kismet/GameplayStatics.h"
#include "Battle/ItemPoint.h"
#include "Engine/World.h"
#include "Item/MasterItem.h"
#include "Battle/BattlePC.h"
#include "Battle/BattleGS.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ConstructorHelpers.h"

ABattleGM::ABattleGM()
{
	//SetReplicates(true);
	//블루프린트 클래스 드래그 로딩(선택), C++ 구현
	static ConstructorHelpers::FClassFinder<AMasterItem> MasterItemClassRef(TEXT("Blueprint'/Game/Blueprints/Item/BP_MasterItem.BP_MasterItem_C'"));
	if (MasterItemClassRef.Succeeded())
	{
		MasterItemClass = MasterItemClassRef.Class;
	}
}

void ABattleGM::BeginPlay()
{
	Super::BeginPlay();

	//class A{}
	// A a = new A();
	//FStringClassReference MasterItemClassRef(TEXT("Blueprint'/Game/Blueprints/MasterItem/BP_MasterItem.BP_MasterItem_C'"));
	//if (UClass* MyClass = MasterItemClassRef.TryLoadClass<AMasterItem>())
	//{
	//	MasterItemClass = MyClass;
	//	UE_LOG(LogClass, Warning, TEXT("Loading"));
	//}


	//아이템 위치 조사
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPoint::StaticClass(), ItemPoints);

	//아이템 생성
	if (MasterItemClass)
	{
		for (auto ItemPoint : ItemPoints)
		{
			GetWorld()->SpawnActor<AMasterItem>(MasterItemClass,
				ItemPoint->GetActorLocation(),
				ItemPoint->GetActorRotation());
		}
	}
}

//void ABattleGM::PreLogin(const FString & Options, const FString & Address, const FUniqueNetIdRepl & UniqueId, FString & ErrorMessage)
//{
//	//ErrorMessage = FString(TEXT("Error"));
//}

//APlayerController * ABattleGM::Login(UPlayer * NewPlayer, ENetRole InRemoteRole, const FString & Portal, const FString & Options, const FUniqueNetIdRepl & UniqueId, FString & ErrorMessage)
//{
//	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
//}

//void ABattleGM::PostLogin(APlayerController * NewPlayer)
//{
//네트워크로 연결이 완료 되어서 통신이 가능 한 시점.
//}

FString ABattleGM::InitNewPlayer(APlayerController * NewPlayerController, const FUniqueNetIdRepl & UniqueId, const FString & Options, const FString & Portal)
{
	//seamless로딩이나 non seamless로딩이나 다 호출 됨.
	//bUseSeamlessTravel = true;
	ABattleGS* GS = GetGameState<ABattleGS>();
	if (GS)
	{
		GS->TotalCount++;
	}

	CheckAliveCount();

	return FString();
}


void ABattleGM::CheckAliveCount()
{
	ABattleGS* GS = GetGameState<ABattleGS>();
	if (GS)
	{
		GS->AliveCount = 0;

		for (auto i = GetWorld()->GetPlayerControllerIterator(); i; ++i)
		{
			ABattlePC* PC = Cast<ABattlePC>(*i);
			if (PC)
			{
				if (!PC->bIsPawnDead)
				{
					GS->AliveCount++;
					GS->OnRep_AliveCount();
					UE_LOG(LogClass, Warning, TEXT("CheckAliveCount %d"), GS->AliveCount);
				}
			}
		}
	}
}

void ABattleGM::CheckEndGame()
{
	ABattleGS* GS = GetGameState<ABattleGS>();
	if (GS)
	{
		if (GS->AliveCount == 1)
		{
			for (auto i = GetWorld()->GetPlayerControllerIterator(); i; ++i)
			{
				ABattlePC* PC = Cast<ABattlePC>(*i);
				if (PC)
				{
					if (!PC->bIsPawnDead)
					{
						PC->S2C_ConclusionChicken(1, GS->TotalCount);
					}
				}
			}

		GetWorld()->GetTimerManager().SetTimer(TimerHandle,
			this, //콜백 함수가 있는 오브젝트
			&ABattleGM::TimerFunction, //콜백 함수 이름
			1.0f);
		}
	}
}



void ABattleGM::ReturnLobby()
{
	GetWorld()->ServerTravel(TEXT("Lobby"));
}

void ABattleGM::TimerFunction()
{
	LeftTime--;

	ABattleGS* GS = GetGameState<ABattleGS>();
	if (GS)
	{
		GS->PublicMessage = FString::Printf(TEXT("게임 종료 까지 남은 시간 \n%d초"), LeftTime);
		if (HasAuthority()) //서버는 값이 바뀌어도 실행이 안됨
		{
			GS->OnRep_PublicMessage();
		}
	}

	if (LeftTime <= 0) //시작 시간 다 되면 시작
	{
		ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		if (PC)
		{
			PC->ReturnLobby(); //로비로 돌아감
		}
	}
	else // 시간 계산 해주기
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,
			this, //콜백 함수가 있는 오브젝트
			&ABattleGM::TimerFunction, //콜백 함수 이름
			1.0f);
	}
}

void ABattleGM::Do0()
{
	GLog->Log(TEXT("탱커가 도망감"));
}

void ABattleGM::Do1(int Number)
{
	GLog->Log(ELogVerbosity::Error, FString::Printf(TEXT("탱커가 도망감 %d초 준다. 어여 와라"), Number));
}

void ABattleGM::Do2(int Number, float Number2)
{
	GLog->Log(FString::Printf(TEXT("%d %f"), Number, Number2));
}

void ABattleGM::Do3(int Number, float Number2, FString String)
{
	GLog->Log(FString::Printf(TEXT("%d %f %s"), Number, Number2, *String));
}
