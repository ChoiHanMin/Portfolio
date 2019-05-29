// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.generated.h"

UCLASS()
class PROJECTBD_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABattleCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ItemGet();

	void Inventory();

	class AMasterItem* GetClosestItem();

	UFUNCTION()
	void TryIronsight();


	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_TryIronsight();
	bool C2S_TryIronsight_Validate();
	void C2S_TryIronsight_Implementation();


	UFUNCTION()
	void TryCrouch();


	UFUNCTION()
	void Turn(float Value);

	UFUNCTION()
	void LookUp(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void MoveForward(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UWeaponComponent* Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UPawnNoiseEmitterComponent* NoiseEmitter;

	UFUNCTION(BlueprintCallable, Category = "Aim")
	FRotator GetAimOffset() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info" , Replicated)
	bool bIsIronsight = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float JogSpeed = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float SprintSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float ProneSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", Replicated)
	bool bIsSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", Replicated)
	bool bIsProne = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", Replicated)
	bool bIsProning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", Replicated)
	bool bIsFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FVector NormalSpringArmPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FVector CrouchSpringArmPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FVector ProneSpringArmPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", Replicated)
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info", Replicated)
	float MaxHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	UAnimMontage* DeadAnimation;


	UFUNCTION(BlueprintCallable)
	void Sprint();

	UFUNCTION(Server, WithValidation, Reliable)
	void C2S_Sprint();
	bool C2S_Sprint_Validate();
	void C2S_Sprint_Implementation();

	UFUNCTION(BlueprintCallable)
	void UnSprint();

	UFUNCTION(Server, WithValidation, Reliable)
	void C2S_UnSprint();
	bool C2S_UnSprint_Validate();
	void C2S_UnSprint_Implementation();



	UFUNCTION(BlueprintCallable)
	void LookAround();

	UFUNCTION(BlueprintCallable)
	void UndoLookAround();


	UFUNCTION(BlueprintCallable)
	void TryProne();

	UFUNCTION(Server, WithValidation, Reliable)
	void C2S_TryProne();
	bool C2S_TryProne_Validate();
	void C2S_TryProne_Implementation();

	UFUNCTION(BlueprintCallable)
	void StartFire();

	UFUNCTION(BlueprintCallable)
	void StopFire();

	UFUNCTION(BlueprintCallable)
	void OnShot();

	UFUNCTION(Server, WithValidation, Reliable)
	void C2S_OnShot(FVector TraceStart, FVector TraceEnd);
	bool C2S_OnShot_Validate(FVector TraceStart, FVector TraceEnd);
	void C2S_OnShot_Implementation(FVector TraceStart, FVector TraceEnd);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_SpawnMuzzleAndSound();
	void S2A_SpawnMuzzleAndSound_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void S2A_SpawnHitEffect(const FHitResult& OutHit);
	void S2A_SpawnHitEffect_Implementation(const FHitResult& OutHit);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UParticleSystem* Explosion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UParticleSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UMaterialInterface* BulletDecal;

	
	FTimerHandle ShootTimeHandle;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	TArray<class AMasterItem*> CanPickupList;

	UFUNCTION(BlueprintCallable)
	void AddPickupItemList(class AMasterItem* Item);

	UFUNCTION(BlueprintCallable)
	void RemovePickupItemList(class AMasterItem* Item);

	void ViewItemTooltip();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void S2A_DeadProcess();
	void S2A_DeadProcess_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void S2A_SetMaxWalkSpeed(float Speed);
	void S2A_SetMaxWalkSpeed_Implementation(float Speed);
};