// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MoveWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMoveWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* AttackButton;
	
	UFUNCTION()
	void Attack();
};
