// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AIController/BTTask_SetMosterState.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_SetMosterState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ANormalMonster* Pawn = Cast<ANormalMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (Pawn)
	{
		Pawn->CurrentState = NewMonsterState;
		Pawn->CurrentAnimState = NewMonsterAnimState;

		OwnerComp.GetBlackboardComponent()->SetValueAsEnum(GetSelectedBlackboardKey(), (uint8)NewMonsterState);

		UE_LOG(LogTemp, Warning, TEXT("Monster State : %s"), *UEnum::GetValueAsString(NewMonsterState));

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
