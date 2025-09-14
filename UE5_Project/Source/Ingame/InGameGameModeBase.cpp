// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Ingame/PortfolioCharacter.h"
#include "Monster/MonsterTargetPoint.h"
#include "GameFramework/PlayerStart.h"
#include "Monster/BallPoint.h"
#include "Monster/NormalMonster.h"
#include "Portfolio/PlayableGameInstance.h"
#include "InGamePlayerController.h"
#include "Engine/World.h"

AInGameGameModeBase::AInGameGameModeBase()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_QuestDataTable(TEXT("/Script/Engine.DataTable'/Game/Resources/CSV/Quest.Quest'"));
	if (DT_QuestDataTable.Succeeded())
	{
		QuestDatatable = DT_QuestDataTable.Object;
	}
}

void AInGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//Get Quest
	if (QuestDatatable)
	{
		QuestDatatable->GetAllRows<FQuestData>(TEXT(""), QuestDataArray);
	}

	//Play BGM
	UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->BGMFadeIn(0.0f, GameInstance->InGameBGMSound);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is null"));
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterTargetPoint::StaticClass(), GoalSpawnPoints);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABallPoint::StaticClass(), MonsterPoint);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartPoint);

	FStringClassReference BallRef(TEXT("/Script/Engine.Blueprint'/Game/Monster/NormalMonster/NormalMonster_BP.NormalMonster_BP_C'"));
	UClass* NormalMonsterClass = BallRef.TryLoadClass<ANormalMonster>();

	for (auto MonsterPoints : MonsterPoint)
	{
		GetWorld()->SpawnActor<ANormalMonster>(NormalMonsterClass, MonsterPoints->GetActorLocation(), MonsterPoints->GetActorRotation());
	}
}

void AInGameGameModeBase::MonsterDead()
{
	MonsterCount++;
	QuestDelegate.ExecuteIfBound(0);
}

void AInGameGameModeBase::HPUpdateInterface_Implementation(float Interface_CurrentHP, float Interface_MaxHP)
{
}

void AInGameGameModeBase::PlayerRespawn(APortfolioCharacter* NowCharacter)
{
	AInGamePlayerController* PlayerController = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//FStringClassReference PlayerCharacter(TEXT("/Script/Engine.Blueprint'/Game/InGame/Character/BP_TopDownCharacter_Clone.BP_TopDownCharacter_Clone_C'"));
	//auto character = PlayerCharacter.TryLoadClass<APortfolioCharacter>();
	//if (character && PlayerStartPoint.Num() > 0)
	//{
	//	FTransform SpawnTransform;
	//	SpawnTransform.SetLocation(PlayerStartPoint[0]->GetActorLocation());
	//	SpawnTransform.SetRotation(PlayerStartPoint[0]->GetActorRotation().Quaternion());
	//	APortfolioCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<APortfolioCharacter>(character, SpawnTransform);
	//	if (NewCharacter)
	//	{
	//		NewCharacter->SetWeponColision(false);
	//		NewCharacter->FinishSpawning(SpawnTransform);
	//		PlayerController->Possess(NewCharacter);
	//		PlayerController->SetViewTargetWithBlend(NewCharacter, 1.5f);
	//	}
	//	NowCharacter->Destroy();
	//}

	if (PlayerStartPoint.Num() > 0)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(PlayerStartPoint[0]->GetActorLocation());
		SpawnTransform.SetRotation(PlayerStartPoint[0]->GetActorRotation().Quaternion());
		
		NowCharacter->SetActorLocationAndRotation(PlayerStartPoint[0]->GetActorLocation(), PlayerStartPoint[0]->GetActorRotation());
		NowCharacter->Respawn();

		PlayerController->SetViewTargetWithBlend(NowCharacter, 1.5f);
	}
}

void AInGameGameModeBase::QuestClear()
{
	AInGamePlayerController* PlayerController = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->ClearGame();
	}
}

void AInGameGameModeBase::PlayerDead()
{
	AInGamePlayerController* PlayerController = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->ChracterDead();
	}
}

