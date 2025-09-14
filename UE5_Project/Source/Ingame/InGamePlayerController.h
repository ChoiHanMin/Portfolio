// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "Ingame/PortfolioCharacter.h"
#include "Character/MyIngameGE.h"
#include "InGamePlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogIngameCharacter, Log, All);
//DECLARE_DELEGATE_OneParam(FDamageDelegate, float);
DECLARE_DELEGATE_TwoParams(FDamageDelegate, float,float);

UCLASS()
class PORTFOLIO_API AInGamePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AInGamePlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetMoveKeyBoard;
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetMoveController;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetAttackInput;

	/** Item Get Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetItemGetInput;

	class UCharacterWidget* CharacterWidget;
	class UIngameWidget* IngameWidget;
	class ULoadingWidget* LoadingWidget;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetWidgetHPPercent(float CurrentHP, float MaxHP);

	FDamageDelegate OnDamage;

	void OnAttack();
	void SetItemGet();
	void LoadingEnd();
	void ChracterDead();
	void ClearGame();

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UMyIngameGE> DamageGE;
protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay() override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	void MoveKeyboard(const FInputActionValue& Value);
	void MoveController(const FInputActionValue& Value);

private:
	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	int32 CurrentComboCount;
	int32 MaxComboCount = 2;
	FTimerHandle ComboTimerHandle;

	void ChangePlayerState(ECharacterState NewState);
	void StartCombo();         
	void ContinueCombo();      
	void EndCombo();      

};
