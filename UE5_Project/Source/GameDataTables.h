// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameDataTables.generated.h"
/**
 *
 */


USTRUCT(BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString Korean = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int CompletNum = 0;
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int ItemIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString ItemName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	float ItemFloat01 = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int ItemCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TAssetPtr<class UTexture2D> ItemThumnail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TAssetPtr<class UStaticMesh> ItemMesh = nullptr;
};

USTRUCT(BlueprintType)
struct PORTFOLIO_API FGameDataTables : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
};