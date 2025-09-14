// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void SetQuestName(const FString& Name, int NowNum, int QuestNum);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UTextBlock* QuestName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UOverlay* QuestClearOverlay;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* ClearAnim;
};
