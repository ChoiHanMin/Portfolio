// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameDataTables.h"
#include "InGameGameModeBase.generated.h"

/**
 *
 */

class APortfolioCharacter;
DECLARE_DELEGATE_OneParam(FQuestDelegate, int32);
UCLASS()
class PORTFOLIO_API AInGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AInGameGameModeBase();
	virtual void BeginPlay() override;

	void MonsterDead();
	FQuestDelegate QuestDelegate;
	static const int GoalPonts = 2;
	int MonsterCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* QuestDatatable;
	TArray<FQuestData*> QuestDataArray;

	TArray<AActor*> GoalSpawnPoints;
	TArray<AActor*> MonsterPoint;
	TArray<AActor*> PlayerStartPoint;
	
	UFUNCTION(BlueprintCallable)	
	void PlayerRespawn(APortfolioCharacter* NowCharacter);
	
	void QuestClear();
	void PlayerDead();
};
