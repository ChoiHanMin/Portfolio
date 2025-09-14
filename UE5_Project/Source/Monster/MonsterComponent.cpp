// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MonsterComponent.h"

// Sets default values for this component's properties
UMonsterComponent::UMonsterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMonsterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMonsterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FMonsterData& UMonsterComponent::GetMonsterData(int Monsterndex)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return *DataTable->FindRow<FMonsterData>(*FString::FromInt(Monsterndex), TEXT("ItemIndex"));
}

