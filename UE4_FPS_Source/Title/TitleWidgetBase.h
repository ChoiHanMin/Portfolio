// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API UTitleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* StartButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UButton* LoginButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UEditableTextBox* UserID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UEditableTextBox* ServerIP;

	UFUNCTION()
	void StartServer();

	UFUNCTION()
	void ConnectServer();

	void SetUserID();

};
