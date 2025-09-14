// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
//#include <Portfolio/Ingame/PortfolioCharacter.h>
#include "Ingame/PortfolioCharacter.h"

#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState CurrentState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterAnimState CurrentAnimState;
};
