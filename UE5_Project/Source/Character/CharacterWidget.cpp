// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Ingame/InGamePlayerController.h"
#include "Kismet/GameplayStatics.h"

void UCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CharWidgetCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CharWidgetCanvas")));
	AttackButton = Cast<UButton>(GetWidgetFromName(TEXT("AttackButton")));
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar_Sec")));
	HPText = Cast<UTextBlock>(GetWidgetFromName(TEXT("HPText")));

	if (AttackButton)
	{
		AttackButton->OnClicked.AddDynamic(this, &UCharacterWidget::Attack);
	}
	GetButton = Cast<UButton>(GetWidgetFromName(TEXT("GetItemButton")));
	if (GetButton)
	{
		GetButton->OnClicked.AddDynamic(this, &UCharacterWidget::GetItem);
	}
}

void UCharacterWidget::Attack()
{
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->OnAttack();
	}
}

void UCharacterWidget::GetItem()
{
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->SetItemGet();
	}
}

void UCharacterWidget::SetHPPercent(float CurrentHP, float MaxHP)
{
	if (HPProgressBar)
	{
		HPProgressBar->SetPercent(CurrentHP / MaxHP);
	}
	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentHP), FMath::RoundToInt(MaxHP))));
	}
}
