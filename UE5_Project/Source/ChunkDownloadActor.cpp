// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkDownloadActor.h"
#include "ChunkDownloader.h"
#include "HAL/PlatformFilemanager.h"
#include "IPlatformFilePak.h"

// Sets default values
AChunkDownloadActor::AChunkDownloadActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChunkDownloadActor::BeginPlay()
{
	Super::BeginPlay();

	Downloader = FChunkDownloader::GetOrCreate();
	Downloader->Initialize(TEXT("Android"), 4);

	StartPatch({1001});
}

// Called every frame
void AChunkDownloadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AChunkDownloadActor::StartPatch(const TArray<int32>& ChunkIDs)
{
	if (!Downloader.IsValid()) return false;

	TargetChunks = ChunkIDs;

	// 다운로드 시작
	TFunction<void(bool)> DownloadComplete = [this](bool bSuccess) {
		if (bSuccess) 
		{
			Downloader->MountChunks(TargetChunks, [this](bool bMountSuccess) 
				{
					OnPatchComplete.Broadcast(bMountSuccess);
				});
		}
		else 
		{
			OnPatchComplete.Broadcast(false);
		}
		};

	Downloader->DownloadChunks(TargetChunks, DownloadComplete, 1);
	return true;
}

