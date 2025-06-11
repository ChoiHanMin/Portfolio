// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Ingame/InGamePlayerController.h"
#include "Kismet/GameplayStatics.h"

void UCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CharWidgetCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CharWidgetCanvas")));
	AttackButton = Cast<UButton>(GetWidgetFromName(TEXT("AttackButton")));
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

void UCharacterWidget::SetPercent(float NewPercent)
{
	Percent = NewPercent;
}
