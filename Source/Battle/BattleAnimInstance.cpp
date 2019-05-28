// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleAnimInstance.h"
#include "Battle/BattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBattleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel())
	{
		Speed = Pawn->GetCharacterMovement()->Velocity.Size();
		Direction = CalculateDirection(Pawn->GetCharacterMovement()->Velocity, Pawn->GetActorRotation());

		bIsCrouch = Pawn->bIsCrouched;

		AimYaw = Pawn->GetAimOffset().Yaw;
		AimPitch = Pawn->GetAimOffset().Pitch;
		bIsIronsight = Pawn->bIsIronsight;
		bIsSprint = Pawn->bIsSprint;
		bIsProne = Pawn->bIsProne;
		bIsProning = Pawn->bIsProning;
		bIsFire = Pawn->bIsFire;
	}
}

void UBattleAnimInstance::AnimNotify_ProneStart(UAnimNotify * Notify)
{
	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel())
	{
		Pawn->bIsProning = true;
	}
}

void UBattleAnimInstance::AnimNotify_ProneEnd(UAnimNotify * Notify)
{
	ABattleCharacter* Pawn = Cast<ABattleCharacter>(TryGetPawnOwner());
	if (Pawn && Pawn->IsValidLowLevel())
	{
		Pawn->bIsProning = false;
	}
}




