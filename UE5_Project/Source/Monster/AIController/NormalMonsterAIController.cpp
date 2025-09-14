// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalMonsterAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include <Portfolio/Monster/NormalMonster.h>


ANormalMonsterAIController::ANormalMonsterAIController()
{
	BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree"));
	BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void ANormalMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ANormalMonster* NormalMonster = Cast<ANormalMonster>(InPawn);
	
	if (NormalMonster)
	{
		BBComponent->InitializeBlackboard(*(NormalMonster->BehaviorTree->BlackboardAsset));
		BTComponent->StartTree(*(NormalMonster->BehaviorTree));
	}

}
