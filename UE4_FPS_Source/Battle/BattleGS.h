// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ABattleGS : public AGameStateBase
{
	GENERATED_BODY()

public:	
	ABattleGS();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_AliveCount)
	int AliveCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TotalCount)
	int TotalCount = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_AliveCount();

	UFUNCTION()
	void OnRep_TotalCount();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_InfoMessage)
	FString InfoMessage;

	UFUNCTION()
	void OnRep_InfoMessage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PublicMessage)
	FString	PublicMessage;

	UFUNCTION()
	void OnRep_PublicMessage();
};
