// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortfolioGameMode.h"
#include "PortfolioPlayerController.h"
#include "Ingame/PortfolioCharacter.h"
#include "UObject/ConstructorHelpers.h"

APortfolioGameMode::APortfolioGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APortfolioPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}