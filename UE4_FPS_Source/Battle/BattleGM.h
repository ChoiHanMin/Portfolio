// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGM.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ABattleGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ABattleGM();

	virtual void BeginPlay() override;

	TArray<AActor *> ItemPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMasterItem> MasterItemClass;

	//virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	//virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	//virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	void CheckAliveCount();

	void CheckEndGame();

	FTimerHandle TimerHandle;

	UFUNCTION()
	void ReturnLobby(); //로비로 돌아가는 타이머 함수

	UFUNCTION()
	void TimerFunction();

	int LeftTime = 10;

	UFUNCTION(Exec)
	void Do0();

	UFUNCTION(Exec)
	void Do1(int Number);

	UFUNCTION(Exec)
	void Do2(int Number, float Number2);

	UFUNCTION(Exec)
	void Do3(int Number, float Number2, FString String);

};

