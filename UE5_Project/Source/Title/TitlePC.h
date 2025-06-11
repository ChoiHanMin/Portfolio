// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePC.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ATitlePC : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	class UTitleWidget* TitleWidget;

	class ULoadingWidget* LoadingWidget;
	void VisibleLoadingWidget();
	//void CloseWidget();
};
