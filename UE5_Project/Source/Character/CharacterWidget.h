// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Percent;

	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* AttackButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* GetButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* CharWidgetCanvas;

	UFUNCTION()
	void Attack();

	UFUNCTION()
	void GetItem();

	void SetPercent(float NewPercent);

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//class UTextBlock* Name;
	//
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//class UTextBlock* HPText;
};
