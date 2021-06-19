// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BattleAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API UBattleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsCrouch;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float AimYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float AimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsIronsight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsProne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsProning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	bool bIsFire;

	UFUNCTION()
	void AnimNotify_ProneStart(UAnimNotify* Notify);

	UFUNCTION()
	void AnimNotify_ProneEnd(UAnimNotify* Notify);
};
