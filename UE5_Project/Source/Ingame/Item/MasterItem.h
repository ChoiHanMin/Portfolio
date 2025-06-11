// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameDataTables.h"
#include "MasterItem.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_API AMasterItem : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	
	AMasterItem();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UItemActorComponent* ItemDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int ItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int ItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	class UWidgetComponent* Widget;

	FItemData* Data;
};
