// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MasterItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API AMasterItem : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AMasterItem(); 

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* Sphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UItemDataTableComponent* ItemDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Replicated)
	int ItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int ItemCount;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
