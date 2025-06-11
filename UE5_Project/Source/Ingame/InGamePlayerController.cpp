// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "IngameCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "character/CharacterWidget.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetTree.h"
#include "IngameWidget.h"
#include "Title/LoadingWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogIngameCharacter);

AInGamePlayerController::AInGamePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}



void AInGamePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AInGamePlayerController::OnInputStarted);
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::OnSetDestinationTriggered);
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AInGamePlayerController::OnSetDestinationReleased);
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AInGamePlayerController::OnSetDestinationReleased);
		//
		//// Setup touch input events
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AInGamePlayerController::OnInputStarted);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AInGamePlayerController::OnTouchTriggered);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AInGamePlayerController::OnTouchReleased);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AInGamePlayerController::OnTouchReleased);



		//Setup 
		EnhancedInputComponent->BindAction(SetMoveKeyBoard, ETriggerEvent::Triggered, this, &AInGamePlayerController::MoveKeyboard);
		EnhancedInputComponent->BindAction(SetMoveController, ETriggerEvent::Triggered, this, &AInGamePlayerController::MoveController);
		EnhancedInputComponent->BindAction(SetAttackInput, ETriggerEvent::Completed, this, &AInGamePlayerController::OnAttack);
		EnhancedInputComponent->BindAction(SetItemGetInput, ETriggerEvent::Completed, this, &AInGamePlayerController::SetItemGet);
	}
	else
	{
		UE_LOG(LogIngameCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}
void AInGamePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//#ifdef PLATFORM_WINDOWS
	//	this->ActivateTouchInterface(nullptr);
	//#endif

	FStringClassReference IngameWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/InGame/IngameWidget_BP.IngameWidget_BP_C'"));
	if (UClass* MyWidgetClass = IngameWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		IngameWidget = Cast<UIngameWidget>(NewObject<UUserWidget>(this, MyWidgetClass));
		IngameWidget->AddToViewport();
	}

	//FStringClassReference LoadingWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Title/LoadingWidget_BP.LoadingWidget_BP_C'"));
	//if (UClass* MyLoadingWidgetClass = LoadingWidgetClassRef.TryLoadClass<UUserWidget>())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Loading Widget On Loading Widget On Loading Widget On Loading Widget On"));
	//	LoadingWidget = Cast<ULoadingWidget>(NewObject<UUserWidget>(this, MyLoadingWidgetClass));
	//	LoadingWidget->AddToViewport();
	//	LoadingWidget->SetVisibility(ESlateVisibility::Visible);
	//	LoadingWidget->PlayIngame();
	//}

	OnDamage.BindUObject(this, &AInGamePlayerController::SetWidgetPercent);
	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	SetInputMode(FInputModeGameAndUI());

	//LoadingEnd();
}

void AInGamePlayerController::SetWidgetPercent(float NewPercent)
{
	if (IngameWidget != nullptr)
	{
		IngameWidget->CharacterWidget->Percent = NewPercent;
	}
}

void AInGamePlayerController::OnInputStarted()
{
	StopMovement();
}

void AInGamePlayerController::MoveKeyboard(const FInputActionValue& Value)
{
	// Get the controlled pawn
	//APawn* ControlledPawn = GetPawn();
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());

	if (ControlledPawn->IsDead())
		return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();


	// find out which way is forward
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
	ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);

}

void AInGamePlayerController::MoveController(const FInputActionValue& Value)
{
	// Get the controlled pawn
	//APawn* ControlledPawn = GetPawn();
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());

	if (ControlledPawn->IsDead())
		return;

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// find out which way is forward
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	ControlledPawn->AddMovementInput(RightDirection, -MovementVector.Y);
	ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.X);
}

void AInGamePlayerController::OnAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack"));

	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());

	if (ControlledPawn->CurrentState == ECharacterState::Normal)
	{
		ControlledPawn->SetWeponColision(true);
		StartCombo();
		return;
	}

	if (ControlledPawn->CurrentState == ECharacterState::Battle)
	{
		ContinueCombo();
		return;
	}

}

void AInGamePlayerController::SetItemGet()
{
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());
	if (ControlledPawn->NearItem)
	{
		if (ControlledPawn->CurrentHP < ControlledPawn->MaxHP)
		{

			ControlledPawn->CurrentHP += ControlledPawn->NearItem->Data->ItemFloat01;
			if (ControlledPawn->CurrentHP > ControlledPawn->MaxHP)
			{
				ControlledPawn->CurrentHP = ControlledPawn->MaxHP;
			}
			ControlledPawn->NearItem->Destroy();
			OnDamage.ExecuteIfBound(ControlledPawn->CurrentHP / ControlledPawn->MaxHP);
		}
		else if (ControlledPawn->CurrentHP >= ControlledPawn->MaxHP)
		{
			IngameWidget->PlayHPFullNotiAnim();
		}
		UE_LOG(LogTemp, Warning, TEXT("Item Near"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Near"));
	}
}

void AInGamePlayerController::LoadingEnd()
{
	if (LoadingWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Play Play Play Play Play Play Play "));
		LoadingWidget->PlayIngame();
	}

	UE_LOG(LogTemp, Warning, TEXT("Loading End"));
}

void AInGamePlayerController::ChangePlayerState(ECharacterState NewState)
{
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->CurrentState = NewState;
	}
}

void AInGamePlayerController::StartCombo()
{
	ChangePlayerState(ECharacterState::Battle);

	CurrentComboCount = 1;

	UE_LOG(LogTemp, Warning, TEXT("Executing Combo %d"), CurrentComboCount);

	// �ִϸ��̼� ���� ���� �߰� ���� (��: ��Ÿ�� ���)
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());
	if (ControlledPawn != nullptr)
	{
		ControlledPawn->CurrentAnimState = ECharacterAnimState::Attack01;
		ControlledPawn->CurrentState = ECharacterState::Battle;

		UE_LOG(LogTemp, Warning, TEXT("State"));
	}

	GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AInGamePlayerController::EndCombo, 0.5f, false);
}

void AInGamePlayerController::ContinueCombo()
{
	if (CurrentComboCount < MaxComboCount)
	{
		CurrentComboCount++;

		ChangePlayerState(ECharacterState::Battle);

		UE_LOG(LogTemp, Warning, TEXT("Executing Combo %d"), CurrentComboCount);

		// �ִϸ��̼� ���� ���� �߰� ���� (��: ��Ÿ�� ���� ����)
		APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());
		if (ControlledPawn != nullptr)
		{
			ControlledPawn->CurrentAnimState = ECharacterAnimState::Attack02;

			UE_LOG(LogTemp, Warning, TEXT("State"));
		}
		GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AInGamePlayerController::EndCombo, 0.5f, false);
	}
}

void AInGamePlayerController::EndCombo()
{
	UE_LOG(LogTemp, Warning, TEXT("Combo Ended"));
	ChangePlayerState(ECharacterState::Normal);
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());
	ControlledPawn->SetWeponColision(false);

	if (ControlledPawn != nullptr)
	{
		ControlledPawn->CurrentAnimState = ECharacterAnimState::Idle;
	}
	CurrentComboCount = 0;

	GetWorld()->GetTimerManager().ClearTimer(ComboTimerHandle);
}

void AInGamePlayerController::ChracterDead()
{
	IngameWidget->ViewDead();
}

void AInGamePlayerController::ClearGame()
{
	IngameWidget->ViewClear();
}

void AInGamePlayerController::OnSetDestinationTriggered()
{
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());

	if (ControlledPawn->IsDead())
		return;

	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer or touch
	//APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AInGamePlayerController::OnSetDestinationReleased()
{
	APortfolioCharacter* ControlledPawn = Cast<APortfolioCharacter>(GetPawn());

	if (ControlledPawn->IsDead())
		return;

	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

void AInGamePlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AInGamePlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
