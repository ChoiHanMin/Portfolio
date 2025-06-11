// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotifyState_AttackState.h"
#include "Ingame/PortfolioCharacter.h"

void UAnimNotifyState_AttackState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_AttackState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UAnimNotifyState_AttackState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	APortfolioCharacter* Pawn = Cast<APortfolioCharacter>(MeshComp->GetOwner());
	if (Pawn)
	{
		Pawn->CurrentAnimState = ECharacterAnimState::Idle;
		Pawn->CurrentState = ECharacterState::Normal;
	}
}