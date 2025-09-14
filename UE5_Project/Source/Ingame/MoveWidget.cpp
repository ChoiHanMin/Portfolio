// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/MoveWidget.h"
#include "Components/Button.h"
#include "Ingame/InGamePlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMoveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	AttackButton = Cast<UButton>(GetWidgetFromName(TEXT("AttackButton")));
}

void UMoveWidget::Attack()
{
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->OnAttack();
	}
}
