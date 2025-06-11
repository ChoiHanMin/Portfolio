// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/IngameWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "character/CharacterWidget.h"
#include "Components/ScrollBox.h"
#include "Ingame/MenuWidget.h"
#include "Ingame/QuestWidget.h"
#include "Ingame/InGamePlayerController.h"
#include "Ingame/InGameGameModeBase.h"
#include "Ingame/PortfolioCharacter.h"
#include "Portfolio/PlayableGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UIngameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	IngameWidgetCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("IngameWidgetCanvas")));
	QuestScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("QuestScrollBox")));
	QuestArray.Empty();
	

	if (IngameWidgetCanvas)
	{
		CreateWidgets();
	}

	RespawnOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("RespawnOverlay")));
	ClearOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("ClearOverlay")));


	RespawnButton = Cast<UButton>(GetWidgetFromName(TEXT("RespawnButton")));
	if (RespawnButton)
	{
		RespawnButton->OnClicked.AddDynamic(this, &UIngameWidget::RespawnCharacter);
	}
	ClearButton = Cast<UButton>(GetWidgetFromName(TEXT("ClearButton")));
	if (ClearButton)
	{
		ClearButton->OnClicked.AddDynamic(this, &UIngameWidget::LoadTitle);
	}

	HPNotiText = Cast<UTextBlock>(GetWidgetFromName(TEXT("HPNotiText")));
	if (HPNotiText)
	{
		HPNotiText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UIngameWidget::CreateWidgets()
{
	FStringClassReference CharacterWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/InGame/Character/CharacterWidget_BP.CharacterWidget_BP_C'"));
	if (UClass* CharHPWidgetClass = CharacterWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		CharacterWidget = Cast<UCharacterWidget>(NewObject<UUserWidget>(this, CharHPWidgetClass));

		IngameWidgetCanvas->AddChild(CharacterWidget);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CharacterWidget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetOffsets(FMargin(0.f));
			CharacterWidget->Percent = 1.0f;
		}
	}

	FStringClassReference MenuWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/InGame/IngameWidgets/MenuWidget_BP.MenuWidget_BP_C'"));
	if (UClass* MenuWidgetClass = MenuWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		MenuWidget = Cast<UMenuWidget>(NewObject<UUserWidget>(this, MenuWidgetClass));
		IngameWidgetCanvas->AddChild(MenuWidget);
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MenuWidget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
			CanvasSlot->SetOffsets(FMargin(0.f));
			MenuWidget->SubMenuPannel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	FStringClassReference QuestWidgetClassRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/InGame/IngameWidgets/QuestWidget_BP.QuestWidget_BP_C'"));
	if (UClass* QuestWidgetClass = QuestWidgetClassRef.TryLoadClass<UUserWidget>())
	{
		AInGameGameModeBase* GM = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			int QuestNum = 0;
			FString IndexName = "";
			for (auto QuestData : GM->QuestDataArray)
			{
				QuestWidget = Cast<UQuestWidget>(NewObject<UUserWidget>(this, QuestWidgetClass));

				if (QuestWidget)
				{
					QuestScrollBox->AddChild(QuestWidget);
					QuestWidget->SetQuestName(*QuestData->Korean, 0, QuestData->CompletNum);
					IndexName = "QuestName_" + FString::FromInt(QuestNum);
					QuestArray.Add(FName(IndexName), QuestWidget);
					UE_LOG(LogTemp, Warning, TEXT("Index Name: %s"), *IndexName);
				}
				QuestNum++;
				QuestScrollBox->AddChild(QuestWidget);
			}

			GM->QuestDelegate.BindUObject(this, &UIngameWidget::SetQuestName);
		}
	}
}

void UIngameWidget::SetQuestName(int Num)
{
	auto GM = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	auto Data = GM->QuestDataArray[Num];
	if (QuestArray.Num() > 0)
	{
		QuestArray[FName(TEXT("QuestName_" + FString::FromInt(Num)))]->SetQuestName(Data->Korean, GM->MonsterCount, Data->CompletNum);
	}
}

void UIngameWidget::ViewDead()
{
	RespawnOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UIngameWidget::ViewClear()
{
	ClearOverlay->SetVisibility(ESlateVisibility::Visible);
}

void UIngameWidget::LoadTitle()
{
	UPlayableGameInstance* GameInstance = Cast<UPlayableGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->BGMFadeOut();
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/Maps/TitleMap"));
}

void UIngameWidget::RespawnCharacter()
{
	AInGameGameModeBase* GM = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	APortfolioCharacter* NowCharacter = Cast<APortfolioCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (GM && NowCharacter)
	{
		GM->PlayerRespawn(NowCharacter);
	}
	RespawnOverlay->SetVisibility(ESlateVisibility::Collapsed);
}

void UIngameWidget::PlayHPFullNotiAnim()
{
	if (HPFullNotiAnim)
	{
		HPNotiText->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(HPFullNotiAnim);
	}
}