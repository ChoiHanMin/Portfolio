// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/Item/ItemNameWidget.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void UItemNameWidget::NativeConstruct()
{
	Super::NativeConstruct();

#if PLATFORM_ANDROID
	MobileItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("MItemName")));
	ItemMobileOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("MobileOverlay")));
	if (ItemMobileOverlay)
	{
		ItemMobileOverlay->SetVisibility(ESlateVisibility::Visible);
	}
#elif PLATFORM_WINDOWS
	PCItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("PCItemName")));
	ItemPCOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("PCOverlay")));
	if (ItemPCOverlay)
	{
		ItemPCOverlay->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Warning, TEXT("ItemPCOverlay is valid"));
	}
#endif

}

void UItemNameWidget::SetName(const FString& Name)
{
#if PLATFORM_ANDROID
	if(MobileItemName==nullptr)
		MobileItemName = Cast<UTextBlock>(GetWidgetFromName(TEXT("MItemName")));
	MobileItemName->SetText(FText::FromString(Name));
#elif PLATFORM_WINDOWS
	PCItemName->SetText(FText::FromString(Name));
	UE_LOG(LogTemp, Warning, TEXT("Name Valid"));
#endif

}

