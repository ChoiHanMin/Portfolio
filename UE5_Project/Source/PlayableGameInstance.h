// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayableGameInstance.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class ELoadedMap : uint8
{
	NONE	= 0 	UMETA(Display = "None"),
	Title			UMETA(Display = "Title"),
	InGame			UMETA(Display = "InGame"),
};

DECLARE_DELEGATE_OneParam(FBGMVolumeDelegate, float);
DECLARE_DELEGATE(FFadeOutDelegate);

UCLASS()
class PORTFOLIO_API UPlayableGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void OnStart() override;

	FBGMVolumeDelegate BGM_Volume_Delegate;
	FFadeOutDelegate FadeOutDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float BGMFadeDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* TitleBGMSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* InGameBGMSound;


	UPROPERTY()
	UAudioComponent* BGMComponent;

	ELoadedMap CurrentLoadMap;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void BGMFadeIn(float StartTime, USoundBase* BGM);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void BGMFadeOut();

	void SetBGMVolume(float Volume);

	ULevelStreaming* StreamingLevel;

	void OpenLevel();
	
	UFUNCTION()
	void GameLoadComplete();

	bool bIsCallLoad = false;

};
