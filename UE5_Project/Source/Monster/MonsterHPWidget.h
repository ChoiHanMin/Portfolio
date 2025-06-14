// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHPWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMonsterHPWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Percent;

	virtual void NativeConstruct() override;
};
