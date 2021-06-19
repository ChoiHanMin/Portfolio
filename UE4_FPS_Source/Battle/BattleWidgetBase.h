// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API UBattleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	class UTextBlock* AliveCount;
	class UTextBlock* Message;
	class UScrollBox* Info;
	class UProgressBar* HPBar;
	class UTextBlock* PublicMessage;

	UFUNCTION()
	void DeleteTimer();

	void AddInfo(FString Message);
	
	
	
};
