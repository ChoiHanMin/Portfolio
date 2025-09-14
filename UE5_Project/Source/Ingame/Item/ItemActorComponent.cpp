// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/Item/ItemActorComponent.h"


// Sets default values for this component's properties
UItemActorComponent::UItemActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ItemDataTable(TEXT("/Script/Engine.DataTable'/Game/Resources/CSV/ItemData.ItemData'"));
	if (DT_ItemDataTable.Succeeded())
	{
		DataTable = DT_ItemDataTable.Object;
	}
}


// Called when the game starts
void UItemActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UItemActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FItemData& UItemActorComponent::GetItemData(int ItemIndex)
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return *DataTable->FindRow<FItemData>(*FString::FromInt(ItemIndex), TEXT("ItemIndex"));
}

