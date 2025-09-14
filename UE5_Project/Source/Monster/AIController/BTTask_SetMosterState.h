// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Monster/NormalMonster.h"
#include "BTTask_SetMosterState.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UBTTask_SetMosterState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	ENormalMonsterState NewMonsterState;

	UPROPERTY(EditAnywhere, Category = Blackboard)
	ENormalMonsterAnimState NewMonsterAnimState;
};
