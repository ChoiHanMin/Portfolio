// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Portfolio/PlayableGameInstance.h"

void ULoadingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EndDelegate.Clear();
	EndDelegate.BindDynamic(this, &ULoadingWidget::LoadingFadeEnd);

	BindToAnimationFinished(LODING_ANIM, EndDelegate);
}

void ULoadingWidget::LoadingFadeEnd()
{
	UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->OpenLevel();
	}
}

void ULoadingWidget::PlayIngame()
{
	PlayAnimation(LOADING_END);
	SetVisibility(ESlateVisibility::Collapsed);
}

void ULoadingWidget::PlayLoading()
{
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(LODING_ANIM);
}
