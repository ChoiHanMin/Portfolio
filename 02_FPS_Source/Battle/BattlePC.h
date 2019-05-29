// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattlePC.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ABattlePC : public APlayerController
{
	GENERATED_BODY()
public:
	ABattlePC();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString UserID;

	UFUNCTION(Server, WithValidation, Reliable)
	void C2S_SetUserID(const FString& NewUserID);
	bool C2S_SetUserID_Validate(const FString& NewUserID);
	void C2S_SetUserID_Implementation(const FString& NewUserID);

	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void S2C_SetupWidget();
	void S2C_SetupWidget_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UItemTooltipWidgetBase* ItemTooltip;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UInventoryWidgetBase* InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBattleWidgetBase* BattleWidget;

	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_DestroyItem(AActor* DestroyItem);
	bool C2S_DestroyItem_Validate(AActor* DestroyItem);
	void C2S_DestroyItem_Implementation(AActor* DestroyItem);

	UFUNCTION(Client, Reliable)
	void S2C_PlayDamageEffect();
	void S2C_PlayDamageEffect_Implementation();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bIsPawnDead = false;

	UFUNCTION(Client, Reliable)
	void S2C_ConclusionChicken(int AliveCount, int TotalCount);
	void S2C_ConclusionChicken_Implementation(int AliveCount, int TotalCount);

	UFUNCTION(Client, Reliable)
	void S2C_ConclusionDie(int AliveCount, int TotalCount);
	void S2C_ConclusionDie_Implementation(int AliveCount, int TotalCount);

	void ReturnLobby();


	class UConclusionWidgetBase* ConclusionWidget;



	class UUserWidget* LoadingWidget;

	UFUNCTION(Client, Reliable)
	void S2C_ViewLoadingScreen();
	void S2C_ViewLoadingScreen_Implementation();
};
