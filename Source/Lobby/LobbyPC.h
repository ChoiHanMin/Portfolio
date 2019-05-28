// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(Client, Reliable)
	void S2C_SetupWidget();
	void S2C_SetupWidget_Implementation();

	class ULobbyWidgetBase* LobbyWidget;

	class UUserWidget* LoadingWidget;

	void StartGame();

	UFUNCTION()
	void TimerStartGame();

	UFUNCTION(Client, Reliable)
	void S2C_ViewLoadingScreen();
	void S2C_ViewLoadingScreen_Implementation();

	FTimerHandle StartGameTimer;

	//1. 입력 받음 메세지를 PC를 통해서 서버에 보냄
	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_SendChatMessage(const FText& Message);
	bool C2S_SendChatMessage_Validate(const FText& Message);
	void C2S_SendChatMessage_Implementation(const FText& Message);

	//2. 서버가 받은 메세지를 전체 유저한테 전달
	UFUNCTION(Client, Reliable)
	void S2C_AddChatMessage(const FText& Message);
	void S2C_AddChatMessage_Implementation(const FText& Message);

};
