// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* MenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* SubMenuCloseButton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* BlurButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* TitleButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UProgressBar* ProgressBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* QuitButton;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCanvasPanel* SubMenuPannel;

	

	UFUNCTION()
	void VisibleMenu();

	UFUNCTION()
	void LoadTitle();

	UFUNCTION()
	void QuitGame();

private:
	bool bIsDragging = false;
	void UpdateProgressBarByMouse(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
};
