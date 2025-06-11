// Fill out your copyright notice in the Description page of Project Settings.


#include "TitlePC.h"
#include "TitleWidget.h"
#include "LoadingWidget.h"

void ATitlePC::BeginPlay()
{
	Super::BeginPlay();

	FStringClassReference TitleWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Title/TitleWidget_BP.TitleWidget_BP_C'"));
	if (UClass* MyWidgetClass = TitleWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		TitleWidget = Cast<UTitleWidget>(NewObject<UUserWidget>(this, MyWidgetClass));
		TitleWidget->AddToViewport();
	}

	UE_LOG(LogTemp, Warning, TEXT("Title Widget Loaded"));

	FStringClassReference LoadingWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Title/LoadingWidget_BP.LoadingWidget_BP_C'"));
	if (UClass* MyLoadingWidgetClass = LoadingWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		LoadingWidget = Cast<ULoadingWidget>(NewObject<UUserWidget>(this, MyLoadingWidgetClass));
		LoadingWidget->AddToViewport();
		LoadingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogTemp, Warning, TEXT("Loading Widget Loaded"));
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}

void ATitlePC::VisibleLoadingWidget()
{
	if (LoadingWidget != nullptr)
	{
		LoadingWidget->PlayLoading();
	}
}