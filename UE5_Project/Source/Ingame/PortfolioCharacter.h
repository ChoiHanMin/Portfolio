// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Ingame/MeshMergeFunctionLibrary.h"
#include "Ingame/Item/MasterItem.h"
#include "PortfolioCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Normal		UMETA(Display = "Normal"),
	Battle		UMETA(Display = "Battle"),
	Hit			UMETA(Display = "Hit"),
	Dead		UMETA(Display = "Dead"),
	Chase		UMETA(Display = "Chase")
};

UENUM(BlueprintType)
enum class ECharacterAnimState : uint8
{
	Idle		UMETA(Display = "Idle"),
	Walk		UMETA(Display = "Walk"),
	Attack01	UMETA(Display = "Attack01"),
	Attack02	UMETA(Display = "Attack02"),
	Death		UMETA(Display = "Death"),
	Hit			UMETA(Display = "Hit"),
	Run			UMETA(Display = "Run")
};


struct FSkeletalMeshMergeParams;
UCLASS(Blueprintable)
class APortfolioCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APortfolioCharacter();
	
	void SetWeponColision(bool bIsOn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterAnimState CurrentAnimState;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	//FSkeletalMeshMergeParams defaultMeshParams;
	
	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float MaxHP = 100.0f;

	bool IsDead();

	AMasterItem* NearItem;
private:

	bool bIsDead=false;
	TArray<FString> MeshPaths =
	{
		TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/ModularBodyParts/Hair02SK.Hair02SK'"),
		TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/ModularBodyParts/Face02SK.Face02SK'"),
		TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/ModularBodyParts/Cloth03SK.Cloth03SK'"),
		TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/ModularBodyParts/Glove01SK.Glove01SK'"),
		TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/ModularBodyParts/Shoe01SK.Shoe01SK'"),
	};

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Weapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WeaponCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Bow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BowArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LeftWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Backpack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RightWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Shield;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* ShieldCollision;


	
	void SetWeaponMesh();

	UFUNCTION()
	void WeaponAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,const FHitResult& SweepResult);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	//UPROPERTY(VisibleAnywhere, Category = "Mesh")
	//class USkeletalMeshComponent* MergedSkeletalMeshComponent;

	//USkeletalMesh* MergeSkeletalMeshes(const TArray<USkeletalMesh*>& MeshesToMerge, USkeleton* Skeleton);
};

