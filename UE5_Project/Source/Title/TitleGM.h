// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TitleGM.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API ATitleGM : public AGameModeBase
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
};
