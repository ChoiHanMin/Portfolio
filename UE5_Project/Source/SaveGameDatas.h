// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameDatas.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API USaveGameDatas : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float BGMVolume = 1.f;
	
	
	
};
