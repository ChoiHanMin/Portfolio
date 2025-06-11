// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/MenuWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/BackgroundBlur.h"
#include "Portfolio/PlayableGameInstance.h"
#include "SaveGameDatas.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SubMenuPannel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("SubMenuPannel")));
	ProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("BGMProgress")));

	if (ProgressBar == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProgressBar is null"));
	}

	MenuButton = Cast<UButton>(GetWidgetFromName(TEXT("MenuButton")));
	if (MenuButton)
	{
		MenuButton->OnClicked.AddDynamic(this, &UMenuWidget::VisibleMenu);
	}

	SubMenuCloseButton = Cast<UButton>(GetWidgetFromName(TEXT("SubMenuCloseButton")));
	if (SubMenuCloseButton)
	{
		SubMenuCloseButton->OnClicked.AddDynamic(this, &UMenuWidget::VisibleMenu);
	}

	TitleButton = Cast<UButton>(GetWidgetFromName(TEXT("TitleButton")));
	if (TitleButton)
	{
		TitleButton->OnClicked.AddDynamic(this, &UMenuWidget::LoadTitle);
	}

	BlurButton = Cast<UButton>(GetWidgetFromName(TEXT("BlurButton")));
	if (BlurButton)
	{
		BlurButton->OnClicked.AddDynamic(this, &UMenuWidget::VisibleMenu);
	}

	QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitButton")));
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMenuWidget::QuitGame);
	}

	SubMenuPannel->SetVisibility(ESlateVisibility::Hidden);
	bIsDragging = false;
}

FReply UMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	//FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	//if (InGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
	//{
		bIsDragging = true;
		UpdateProgressBarByMouse(InGeometry, InMouseEvent);
		return FReply::Handled().CaptureMouse(TakeWidget());
	//}

	//return FReply::Unhandled();
}

FReply UMenuWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (bIsDragging)
	{
		UpdateProgressBarByMouse(InGeometry, InMouseEvent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UMenuWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	FAsyncSaveGameToSlotDelegate SavedDelegate;
	SavedDelegate.BindLambda([this](const FString& SlotName, int32 UserIndex, bool bSuccess)
		{
			if (bSuccess)
			{
				UE_LOG(LogTemp, Warning, TEXT("Save Sucess: %s"), *SlotName);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Save Failed"));
			}
		});

	USaveGameDatas* SaveGameInstance = Cast<USaveGameDatas>(UGameplayStatics::CreateSaveGameObject(USaveGameDatas::StaticClass()));

	SaveGameInstance->BGMVolume = ProgressBar->Percent;

	UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, TEXT("SavedDatas"), 0, SavedDelegate);

	bIsDragging = false;
	return FReply::Handled().ReleaseMouseCapture();
}

void UMenuWidget::UpdateProgressBarByMouse(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ProgressBar)
	{
		FVector2D LocalPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		float BarWidth = InGeometry.GetLocalSize().X;
		float Percent = FMath::Clamp(LocalPos.X / BarWidth, 0.0f, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Percent: %f"), Percent);
		ProgressBar->SetPercent(Percent);

		UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->BGM_Volume_Delegate.ExecuteIfBound(Percent);
		}
	}
}

void UMenuWidget::VisibleMenu()
{
	if (SubMenuPannel)
	{
		bool bIsVisible = SubMenuPannel->IsVisible();
		if (USaveGameDatas* LoadedGame = Cast<USaveGameDatas>(UGameplayStatics::LoadGameFromSlot(TEXT("SavedDatas"), 0)))
		{
			UE_LOG(LogTemp, Warning, TEXT("LOADED: %f"), LoadedGame->BGMVolume);

			ProgressBar->SetPercent(LoadedGame->BGMVolume);
		}
		else
		{
			ProgressBar->SetPercent(1.0f);
		}

		SubMenuPannel->SetVisibility(bIsVisible ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SubMenuPannel is null"));
	}
}

void UMenuWidget::LoadTitle()
{
	UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OpenLevel();
	}
}

void UMenuWidget::QuitGame()
{
	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		return;
	}

	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, true);
}

