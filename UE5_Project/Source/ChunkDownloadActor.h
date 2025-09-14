// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkDownloadActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPatchComplete, bool, bSuccess);

class FChunkDownloader;
UCLASS()
class PORTFOLIO_API AChunkDownloadActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkDownloadActor();

private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TSharedPtr<FChunkDownloader> Downloader;
	TArray<int32> TargetChunks;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool StartPatch(const TArray<int32>& ChunkIDs);

	UPROPERTY(BlueprintAssignable)
	FOnPatchComplete OnPatchComplete;
};
