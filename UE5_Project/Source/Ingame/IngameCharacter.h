// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IngameCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState_1 : uint8
{
	Normal		UMETA(Display = "Normal"),
	Battle		UMETA(Display = "Battle"),
	Hit			UMETA(Display = "Hit"),
	Dead		UMETA(Display = "Dead"),
	Chase		UMETA(Display = "Chase")
};

UENUM(BlueprintType)
enum class ECharacterAnimState_1 : uint8
{
	Idle		UMETA(Display = "Idle"),
	Walk		UMETA(Display = "Walk"),
	Attack01	UMETA(Display = "Attack01"),
	Attack02	UMETA(Display = "Attack02"),
	Death		UMETA(Display = "Death"),
	Hit			UMETA(Display = "Hit"),
	Run			UMETA(Display = "Run")
};


class UInputMappingContext;
class UInputAction;

UCLASS(Blueprintable)
class PORTFOLIO_API AIngameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIngameCharacter();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState_1 CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterAnimState_1 CurrentAnimState;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetMoveKeyBoard;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Weapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Bow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BowArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LeftWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Backpack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RightWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Shield;

	

};
