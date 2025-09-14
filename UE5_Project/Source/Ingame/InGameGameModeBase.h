// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UObject/Interface.h"
#include "GameDataTables.h"
#include "InGameGameModeBase.generated.h"

/**
 *
 */

 // 리플렉션을 위한 빈 클래스 (U 접두사)
UINTERFACE(MinimalAPI, Blueprintable)
class UHPUpdateInterface : public UInterface {
	GENERATED_BODY()
};

// 실제 인터페이스 클래스 (I 접두사)
class IHPUpdateInterface {
	GENERATED_BODY()

public:
	// 순수 가상 함수 선언
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MyCategory")
	void HPUpdateInterface(float Interface_CurrentHP, float Interface_MaxHP);
};

class APortfolioCharacter;
DECLARE_DELEGATE_OneParam(FQuestDelegate, int32);
UCLASS()
class PORTFOLIO_API AInGameGameModeBase : public AGameModeBase, public IHPUpdateInterface
{
	GENERATED_BODY()
public:
	AInGameGameModeBase();
	virtual void BeginPlay() override;

	void MonsterDead();

	FQuestDelegate QuestDelegate;
	
	// 인터페이스 함수 오버라이드
	virtual void HPUpdateInterface_Implementation(float Interface_CurrentHP, float Interface_MaxHP) override;

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
