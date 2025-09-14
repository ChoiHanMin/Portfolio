// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/Item/MasterItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Ingame/Item/ItemActorComponent.h"
#include "Engine/StreamableManager.h"
#include "Components/WidgetComponent.h"
#include "Ingame/Item/ItemNameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Ingame/PortfolioCharacter.h"


AMasterItem::AMasterItem()
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetStaticMeshComponent());
	Sphere->SetSphereRadius(30.0f);

	ItemDataTable = CreateDefaultSubobject<UItemActorComponent>(TEXT("ItemDataTable"));

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemNameWidget"));
	Widget->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UItemNameWidget> Widget_Class(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/InGame/Items/ItemNameWidget_BP.ItemNameWidget_BP_C'"));
	if (Widget_Class.Succeeded())
	{
		Widget->SetWidgetClass(Widget_Class.Class);
	}
}

void AMasterItem::BeginPlay()
{
	Super::BeginPlay();               
	if (ItemDataTable && ItemDataTable->DataTable)
	{
		ItemIndex = FMath::RandRange(1, 2);
		Data = &ItemDataTable->GetItemData(ItemIndex);
		ItemCount = Data->ItemCount;

		FStreamableManager AssetLoader;
		GetStaticMeshComponent()->SetStaticMesh(AssetLoader.LoadSynchronous<UStaticMesh>(Data->ItemMesh));

		UItemNameWidget* ItemWidget = Cast<UItemNameWidget>(Widget->GetUserWidgetObject());
		if (ItemWidget)
		{
			ItemWidget->SetName(Data->ItemName);
		}

		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMasterItem::OnBeginOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AMasterItem::OnEndOverlap);
	}
}

void AMasterItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FVector CameraLocation;
	FRotator CameraRotation;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Dir = CameraLocation - Widget->GetComponentLocation();
	Widget->SetWorldRotation(Dir.Rotation());
}

void AMasterItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		APortfolioCharacter* Pawn = Cast<APortfolioCharacter>(OtherActor);
		if (Pawn)
		{
			Pawn->NearItem = this;
			UE_LOG(LogClass, Warning, TEXT("Character item name "));
		}
	}
}

void AMasterItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		APortfolioCharacter* Pawn = Cast<APortfolioCharacter>(OtherActor);
		if (Pawn && Pawn->NearItem != nullptr)
		{
			Pawn->NearItem = nullptr;
			UE_LOG(LogClass, Warning, TEXT("Remove"));

		}
	}
}
