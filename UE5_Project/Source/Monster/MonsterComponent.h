// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MonsterComponent.generated.h"

UENUM(BlueprintType)
enum class EEXPType : uint8
{
	Normal = 0				UMETA(Display = "Normal"),
	Super = 1				UMETA(Display = "Super"),
	Boss = 2				UMETA(Display = "Boss")
};

USTRUCT(BlueprintType)
struct FMonsterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FMonsterData()
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	FString Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MaxHP = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float AttackPower = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MoveSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	EEXPType EXPType = EEXPType::Normal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TAssetPtr<class ACharacter> Monster = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTFOLIO_API UMonsterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMonsterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable")
	class UDataTable* DataTable;

	UFUNCTION(BlueprintCallable, Category = "DataTable")
	FMonsterData& GetMonsterData(int Monsterndex);
};
