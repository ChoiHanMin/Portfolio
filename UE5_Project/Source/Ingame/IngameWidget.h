// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UIngameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void CreateWidgets();
	UFUNCTION()
	void SetQuestName(int Num);
	
	void ViewDead();
	void ViewClear();

	UFUNCTION()
	void LoadTitle();
	UFUNCTION()
	void RespawnCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* IngameWidgetCanvas;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UScrollBox* QuestScrollBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UOverlay* RespawnOverlay;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UOverlay* ClearOverlay;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* RespawnButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* ClearButton;


	class UCharacterWidget* CharacterWidget;
	class UMenuWidget* MenuWidget;
	class UQuestWidget* QuestWidget;
	class ULoadingWidget* LoadingWidget;

	TMap<FName, UQuestWidget*> QuestArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTextBlock* HPNotiText;
	
	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* HPFullNotiAnim;
	
	void PlayHPFullNotiAnim();
	
};
