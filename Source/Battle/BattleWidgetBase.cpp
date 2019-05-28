// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/ProgressBar.h"
#include "Battle/BattleCharacter.h"
#include "TimerManager.h"

void UBattleWidgetBase::NativeConstruct()
{
	AliveCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("AliveCount")));
	Message = Cast<UTextBlock>(GetWidgetFromName(TEXT("Message")));
	Info = Cast<UScrollBox>(GetWidgetFromName(TEXT("Info")));
	HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HPBar")));
	PublicMessage = Cast<UTextBlock>(GetWidgetFromName(TEXT("PublicMessage")));
}

void UBattleWidgetBase::NativeTick(const FGeometry & MyGeometry, float InDeltaTime)
{
	ABattleCharacter* Pawn = Cast<ABattleCharacter>(GetOwningPlayerPawn());
	if (Pawn && HPBar)
	{
		HPBar->SetPercent(Pawn->CurrentHP / Pawn->MaxHP);
	}
	else //관전자 모드 일때는 업데이트를 못함
	{
		HPBar->SetPercent(0);
	}
}

void UBattleWidgetBase::DeleteTimer()
{
	if (Info)
	{
		if (Info->GetChildrenCount() > 0)
		{
			Info->RemoveChildAt(0);
		}
	}
}

void UBattleWidgetBase::AddInfo(FString Message)
{
	if (Info)
	{
		UTextBlock* NewMessage = NewObject<UTextBlock>(Info);
		if (NewMessage)
		{
			NewMessage->SetJustification(ETextJustify::Right);
			NewMessage->SetText(FText::FromString(Message));
			Info->AddChild(NewMessage);

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle,
				this,
				&UBattleWidgetBase::DeleteTimer,
				3.0f,
				false
			);
		}
	}
}