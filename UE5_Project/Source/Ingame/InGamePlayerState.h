// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Character/PlayerAttributeSet.h"
#include "InGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AInGamePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AInGamePlayerState();

	/** AbilitySystemComponent (네트워크 복제 지원) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UPlayerAttributeSet* AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
};
