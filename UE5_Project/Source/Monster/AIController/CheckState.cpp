// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AIController/CheckState.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Monster/AIController/NormalMonsterAIController.h"
#include "Monster/NormalMonster.h"
#include "Ingame/PortfolioCharacter.h"


void UCheckState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	ANormalMonster* Monster = Cast<ANormalMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (Monster)
	{
		switch (Monster->CurrentState)
		{
		case ENormalMonsterState::Battle:
		{
			Monster->SetAttackColision(true);
			APortfolioCharacter* Player = Cast<APortfolioCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("Target"))));

			float Range = FVector::Distance(Monster->GetActorLocation(), Player->GetActorLocation());
			FVector Dir = Player->GetActorLocation() - Monster->GetActorLocation();
			//플레이어 바라보기
			Monster->SetActorRotation(Dir.Rotation());

			//공격 범위보다 멀어지면 Chase 상태로 전환
			if (Range > Monster->AttackRange)
			{
				Monster->CurrentState = ENormalMonsterState::Chase;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Run;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("CurrentState", (uint8)Monster->CurrentState);
			}
			//플레이어가 죽으면 Normal 상태로 전환
			else if (Player->GetCurrentHP() <= 0)
			{
				Monster->CurrentState = ENormalMonsterState::Normal;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Idle;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("CurrentState", (uint8)Monster->CurrentState);
			}

		}
		break;

		case ENormalMonsterState::Chase:
		{

			if (Monster->CurrentHP <= 0 && Monster->CurrentState != ENormalMonsterState::Dead)
			{
				Monster->CurrentState = ENormalMonsterState::Dead;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Death;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("CurrentState", (uint8)Monster->CurrentState);
				return;
			}

			AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FName(TEXT("Target"))));
			if (Player == nullptr)
			{
				Monster->CurrentState = ENormalMonsterState::Normal;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Idle;
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(FName(TEXT("CurrentState")), (uint8)Monster->CurrentState);
				return;
			}

			float Range = FVector::Distance(Monster->GetActorLocation(), Player->GetActorLocation());

			// 범위 안에 있으면 공격
			if (Range <= Monster->AttackRange)
			{
				Monster->CurrentState = ENormalMonsterState::Battle;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Attack01;
				
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(FName(TEXT("CurrentState")), (uint8)Monster->CurrentState);
			}
			// 볼 수 있는 범위를 벗어나면 Normal 상태로 전환
			else if (Range > Monster->PawnSensing->SightRadius)
			{
				Monster->CurrentState = ENormalMonsterState::Normal;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Idle;
				
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(FName(TEXT("CurrentState")), (uint8)Monster->CurrentState);
			}
			else
			{
				Monster->CurrentState = ENormalMonsterState::Normal;
				Monster->CurrentAnimState = ENormalMonsterAnimState::Idle;
			
				OwnerComp.GetBlackboardComponent()->SetValueAsEnum(FName(TEXT("CurrentState")), (uint8)Monster->CurrentState);
			}
		}
		break;

		case ENormalMonsterState::Dead:
			OwnerComp.GetBlackboardComponent()->SetValueAsEnum("CurrentState", (uint8)ENormalMonsterState::Dead);
			break;
		}
	}
}
