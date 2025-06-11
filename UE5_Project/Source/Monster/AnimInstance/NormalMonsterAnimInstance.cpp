// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalMonsterAnimInstance.h"

void UNormalMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ANormalMonster* Pawn = Cast<ANormalMonster>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevelFast())
	{
		CurrentAnimState = Pawn->CurrentAnimState;
		CurrentState = Pawn->CurrentState;
	}
}
