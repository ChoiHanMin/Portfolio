// Fill out your copyright notice in the Description page of Project Settings.

#include "ConclusionWidgetBase.h"
#include "Components/TextBlock.h"

void UConclusionWidgetBase::NativeConstruct()
{
	UserID = Cast<UTextBlock>(GetWidgetFromName(TEXT("UserID")));
	Message = Cast<UTextBlock>(GetWidgetFromName(TEXT("Message")));
	Ranking = Cast<UTextBlock>(GetWidgetFromName(TEXT("Ranking")));
	TotalCount = Cast<UTextBlock>(GetWidgetFromName(TEXT("TotalCount")));
	PublicMessage = Cast<UTextBlock>(GetWidgetFromName(TEXT("PublicMessage")));
}

void UConclusionWidgetBase::LoseConclusion(const int AliveCount, const int TotalCountNum, const FString & LoseUserID)
{
	Message->SetText(FText::FromString(TEXT("그럴 수 있어. 이런 날도 있는 거지 뭐.")));
	UserID->SetText(FText::FromString(LoseUserID));
	Ranking->SetText(FText::FromString(FString::Printf(TEXT("#%d"), AliveCount)));
	TotalCount->SetText(FText::FromString(FString::Printf(TEXT("/%d"), TotalCountNum)));
}

void UConclusionWidgetBase::WinConclusion(const int AliveCount, const int TotalCountNum, const FString & WinUserID)
{
	Message->SetText(FText::FromString(TEXT("이겼닭! 오늘 저녘은 치킨이닭!")));
	UserID->SetText(FText::FromString(WinUserID));
	Ranking->SetText(FText::FromString(FString::Printf(TEXT("#1"))));
	TotalCount->SetText(FText::FromString(FString::Printf(TEXT("/%d"), TotalCountNum)));
}
