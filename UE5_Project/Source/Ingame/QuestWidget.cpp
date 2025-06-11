// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/QuestWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

void UQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();
	QuestClearOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("QuestClearOverLAY")));
	QuestName = Cast<UTextBlock>(GetWidgetFromName(TEXT("QuestName")));
}

void UQuestWidget::SetQuestName(const FString& Name, int NowNum, int QuestNum)
{
	if (QuestName)
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget Quest Name: %s"), *Name);
		FText QuestText = FText::Format(FText::FromString("{0} {1}/{2}"), FText::FromString(*Name), FText::AsNumber(NowNum), FText::AsNumber(QuestNum));
		QuestName->SetText(QuestText);
	}

	if (NowNum == QuestNum)
	{
		if (QuestClearOverlay)
		{
			QuestClearOverlay->SetVisibility(ESlateVisibility::Visible);
			PlayAnimation(ClearAnim);
		}
	}
}

