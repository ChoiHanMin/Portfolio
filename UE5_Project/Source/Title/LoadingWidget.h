// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LODING_ANIM;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* LOADING_END;
	

	UFUNCTION()
	void LoadingFadeEnd();

	void PlayIngame();
	void PlayLoading();

private:
	FWidgetAnimationDynamicEvent EndDelegate;
};
