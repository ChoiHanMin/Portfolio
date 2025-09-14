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

 // ���÷����� ���� �� Ŭ���� (U ���λ�)
UINTERFACE(MinimalAPI, Blueprintable)
class UHPUpdateInterface : public UInterface {
	GENERATED_BODY()
};

// ���� �������̽� Ŭ���� (I ���λ�)
class IHPUpdateInterface {
	GENERATED_BODY()

public:
	// ���� ���� �Լ� ����
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
	
	// �������̽� �Լ� �������̵�
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
