// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ingame/Item/MasterItem.h"
#include "Character/MyIngameGE.h"
#include "NormalMonster.generated.h"

UENUM(BlueprintType)
enum class ENormalMonsterState : uint8
{
	Normal		UMETA(Display = "Normal"),
	Battle		UMETA(Display = "Battle"),
	Hit			UMETA(Display = "Hit"),
	Dead		UMETA(Display = "Dead"),
	Chase		UMETA(Display = "Chase")
};

UENUM(BlueprintType)
enum class ENormalMonsterAnimState : uint8
{
	Idle		UMETA(Display = "Idle"),
	Walk		UMETA(Display = "Walk"),
	Attack01	UMETA(Display = "Attack01"),
	Attack02	UMETA(Display = "Attack02"),
	Death		UMETA(Display = "Death"),
	Hit			UMETA(Display = "Hit"),
	Run			UMETA(Display = "Run")
};

UCLASS()
class PORTFOLIO_API ANormalMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANormalMonster();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENormalMonsterState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENormalMonsterAnimState CurrentAnimState;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMasterItem> DropItem;

	float DissolveTimeElapsed = 0.0f;
	float DissolveDuration = 0.0f;
	bool bIsDissolving = false;

	FTimerHandle DissolveTimerHandle;
	void UpdateDissolve();

	UFUNCTION(BlueprintCallable, Category = "Dissolve")
	void StartDissolve(float Duration = 3.0f); 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UMyIngameGE> DamageGE;

	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
	class UWidgetComponent* Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float MaxHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float RunSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UParticleSystem* DeadEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* AttackCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	class UPawnSensingComponent* PawnSensing;

	UFUNCTION()
	void NormalAttack(AActor* Attacker, AActor* TargetActor, float DamageAmount);

	void SetAttackColision(bool bIsOn);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Item
	void SpawnItem();
};
