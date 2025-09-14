// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API UItemNameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UTextBlock* PCItemName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UTextBlock* MobileItemName;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UOverlay* ItemPCOverlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UOverlay* ItemMobileOverlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Quest")
	class UButton* MobileGetButton;
	
	void SetName(const FString& Name);
};
