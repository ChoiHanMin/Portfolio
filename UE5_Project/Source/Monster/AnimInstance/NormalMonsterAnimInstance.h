// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include <Portfolio/Monster/NormalMonster.h>
#include "NormalMonsterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UNormalMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENormalMonsterState CurrentState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENormalMonsterAnimState CurrentAnimState;
};
