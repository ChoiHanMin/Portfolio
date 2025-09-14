// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NormalMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ANormalMonsterAIController : public AAIController
{
	GENERATED_BODY()
public:
	ANormalMonsterAIController();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBehaviorTreeComponent* BTComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBlackboardComponent* BBComponent;

	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;
};
