// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Basic/WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"
#include "Basic/ShootCameraShake.h"
#include "Basic/BulletDamageType.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Battle/BattlePC.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Item/MasterItem.h"
#include "UI/ItemTooltipWidgetBase.h"
#include "Components/TextBlock.h"
#include "Item/ItemDataTableComponent.h"
#include "UI/ItemSlotWidgetBase.h"
#include "UI/InventoryWidgetBase.h"
#include "UnrealNetwork.h"
#include "Battle/BattleGS.h"
#include "Battle/BattleGM.h"

// Sets default values
ABattleCharacter::ABattleCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//카메라 스프링암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	//스프링 암 위치 조정
	SpringArm->SetRelativeLocation(FVector(0, 30, 70));
	//스프링 암 길이 조정
	SpringArm->TargetArmLength = 150.0f;

	//카메라 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	//무기 메시 컴포넌트 생성
	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	//소켓에 연결 하기
	Weapon->SetupAttachment(GetMesh(), TEXT("RHandWeapon"));
	//기본 메시 이동
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88.0f), FRotator(0, -90, 0));

	//스프링암을 절대 회전으로 회전 시킴
	SpringArm->bUsePawnControlRotation = true;

	//애니메이션을 애니메이션 블루프린트를 사용하는것으로 설정
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//애니메이션 블루프린트 클래스 로딩, 애님 블루프린트명 뒤에 _C
	static ConstructorHelpers::FClassFinder<UAnimInstance> Anim_Class(TEXT("AnimBlueprint'/Game/Blueprints/Battle/ABP_Battle_Male.ABP_Battle_Male_C'"));
	if (Anim_Class.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(Anim_Class.Class);
	}

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeedCrouched = WalkSpeed;
	GetCharacterMovement()->CrouchedHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();


	NormalSpringArmPosition = SpringArm->GetRelativeTransform().GetLocation();
	CrouchSpringArmPosition = FVector(NormalSpringArmPosition.X, NormalSpringArmPosition.Y, NormalSpringArmPosition.Z - 40);
	ProneSpringArmPosition = FVector(100, NormalSpringArmPosition.Y, NormalSpringArmPosition.Z - 100);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_Explosion(TEXT("ParticleSystem'/Game/Effects/P_AssaultRifle_MF.P_AssaultRifle_MF'"));
	if (P_Explosion.Succeeded())
	{
		Explosion = P_Explosion.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_HitEffect(TEXT("ParticleSystem'/Game/Effects/P_AssaultRifle_IH.P_AssaultRifle_IH'"));
	if (P_HitEffect.Succeeded())
	{
		HitEffect = P_HitEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> S_Explosion(TEXT("SoundCue'/Game/Sound/Weapons/SMG_Thompson/Cue_Thompson_Shot.Cue_Thompson_Shot'"));
	if (S_Explosion.Succeeded())
	{
		ExplosionSound = S_Explosion.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_BloodEffect(TEXT("ParticleSystem'/Game/Effects/P_body_bullet_impact.P_body_bullet_impact'"));
	if (P_BloodEffect.Succeeded())
	{
		BloodEffect = P_BloodEffect.Object;
	}


	static ConstructorHelpers::FObjectFinder<UAnimMontage> Anim_Dead(TEXT("AnimMontage'/Game/Male_Grunt/Animations/Death_1_Montage.Death_1_Montage'"));
	if (Anim_Dead.Succeeded())
	{
		DeadAnimation = Anim_Dead.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> M_BulletDecal(TEXT("Material'/Game/Effects/Decal/M_BulletDecal.M_BulletDecal'"));
	if (M_BulletDecal.Succeeded())
	{
		BulletDecal = M_BulletDecal.Object;
	}

	Tags.Add(FName(TEXT("Player")));

	NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;

	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
}

// Called every frame
void ABattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogClass, Warning, TEXT("%f"), SpringArm->GetComponentLocation().Z);

	if (bIsSprint && GetCharacterMovement()->Velocity.SizeSquared() == 0)
	{
		bIsSprint = false;
	}
}

// Called to bind functionality to input
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABattleCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABattleCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABattleCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABattleCharacter::MoveForward);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABattleCharacter::TryCrouch);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &ABattleCharacter::TryIronsight);
	PlayerInputComponent->BindAction(TEXT("Prone"), IE_Pressed, this, &ABattleCharacter::TryProne);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ABattleCharacter::Sprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ABattleCharacter::UnSprint);

	PlayerInputComponent->BindAction(TEXT("LookAround"), IE_Pressed, this, &ABattleCharacter::LookAround);
	PlayerInputComponent->BindAction(TEXT("LookAround"), IE_Released, this, &ABattleCharacter::UndoLookAround);


	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ABattleCharacter::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ABattleCharacter::StopFire);

	PlayerInputComponent->BindAction(TEXT("ItemGet"), IE_Released, this, &ABattleCharacter::ItemGet);
	PlayerInputComponent->BindAction(TEXT("Inventory"), IE_Released, this, &ABattleCharacter::Inventory);

}

void ABattleCharacter::ItemGet()
{
	AMasterItem* PickupItem = GetClosestItem();
	if (PickupItem && !PickupItem->IsPendingKill())
	{
		RemovePickupItemList(PickupItem);
		ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC)
		{
			UItemSlotWidgetBase* Slot = PC->InventoryWidget->GetSameIDSlot(PickupItem->ItemIndex);
			if (Slot)
			{
				Slot->AddItemCount(PickupItem->ItemCount);
				PC->C2S_DestroyItem(PickupItem);
				ViewItemTooltip();
			}
			else
			{
				UItemSlotWidgetBase* Slot = PC->InventoryWidget->GetEmptySlot();
				if (Slot)
				{
					Slot->SetItem(PickupItem->ItemIndex);
					PC->C2S_DestroyItem(PickupItem);
					ViewItemTooltip();
				}
				else
				{
					UE_LOG(LogClass, Warning, TEXT("Inventory full."))
				}
			}
		}
	}
}

void ABattleCharacter::Inventory()
{
	ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->ToggleInventory();
	}
}

AMasterItem* ABattleCharacter::GetClosestItem()
{
	AMasterItem* ClosestItem = nullptr;
	float Min = 9999999999.9f;

	for (auto Item : CanPickupList)
	{
		float Distance = FVector::Distance(Item->GetActorLocation(), GetActorLocation());
		if (Min > Distance)
		{
			Min = Distance;
			ClosestItem = Item;
		}
	}

	return ClosestItem;
}

void ABattleCharacter::TryIronsight()
{
	C2S_TryIronsight();
}


void ABattleCharacter::C2S_TryIronsight_Implementation()
{
	if (bIsSprint)
	{
		return;
	}

	bIsIronsight = bIsIronsight ? false : true;

	if (!bIsIronsight)
	{
		S2A_SetMaxWalkSpeed(JogSpeed);
	}
	else
	{
		S2A_SetMaxWalkSpeed(WalkSpeed);
	}
}

bool ABattleCharacter::C2S_TryIronsight_Validate()
{
	return true;
}


void ABattleCharacter::TryCrouch()
{
	if (bIsProning) //엎드리는 중에는 상태 변화 안됨
	{
		return;
	}

	if (CanCrouch() && !bIsSprint && !bIsProne) //서있을때
	{
		Crouch();
	}
	else if (bIsProne) //기어 다닐때
	{
		bIsProne = false;
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void ABattleCharacter::Turn(float Value)
{
	if (Value != 0.0)
	{
		AddControllerYawInput(Value);
	}
}

void ABattleCharacter::LookUp(float Value)
{
	if (Value != 0.0)
	{
		AddControllerPitchInput(-Value);
	}
}

void ABattleCharacter::MoveRight(float Value)
{
	if (Value != 0.0 && !bIsSprint && !bIsProning)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}


void ABattleCharacter::MoveForward(float Value)
{
	if (Value != 0.0 && !bIsProning)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

FRotator ABattleCharacter::GetAimOffset() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void ABattleCharacter::Sprint()
{
	C2S_Sprint();
}

bool ABattleCharacter::C2S_Sprint_Validate()
{
	return true;
}


void ABattleCharacter::C2S_Sprint_Implementation()
{
	if (!bIsCrouched && !bIsIronsight && !bIsProne && !bIsProning &&
		!bIsFire && GetCharacterMovement()->Velocity.Size() > 0)
	{
		bIsSprint = true;
		S2A_SetMaxWalkSpeed(SprintSpeed);
	}
}

void ABattleCharacter::UnSprint()
{
	C2S_UnSprint();
}


bool ABattleCharacter::C2S_UnSprint_Validate()
{
	return true;
}

void ABattleCharacter::C2S_UnSprint_Implementation()
{
	if (bIsSprint)
	{
		bIsSprint = false;
		S2A_SetMaxWalkSpeed(JogSpeed);
	}
}

void ABattleCharacter::LookAround()
{
	if (!bIsIronsight)
	{
		bUseControllerRotationYaw = false;
	}

}

void ABattleCharacter::UndoLookAround()
{
	bUseControllerRotationYaw = true;
}

void ABattleCharacter::TryProne()
{
	C2S_TryProne();
}


bool ABattleCharacter::C2S_TryProne_Validate()
{
	return true;
}

void ABattleCharacter::C2S_TryProne_Implementation()
{
	if (bIsProning)
	{
		return;
	}

	if (!bIsSprint && !bIsProne)
	{
		bIsProne = true;
		UnCrouch();
		S2A_SetMaxWalkSpeed(ProneSpeed);
	}
	else if (bIsProne)
	{
		bIsProne = false;
		S2A_SetMaxWalkSpeed(JogSpeed);
		UnCrouch();
	}
}

void ABattleCharacter::StartFire()
{
	if (!bIsSprint && !bIsProning)
	{
		bIsFire = true;
		OnShot();
	}
}

void ABattleCharacter::StopFire()
{
	bIsFire = false;
}

void ABattleCharacter::OnShot()
{
	if (!bIsFire)
	{
		return;
	}

	FVector CameraLocation;
	FRotator CameraRotation;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerViewPoint(CameraLocation, CameraRotation);

	int SizeX;
	int SizeY;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewportSize(SizeX, SizeY);

	FVector WorldLocation;
	FVector WorldDirection;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectScreenPositionToWorld(SizeX / 2, SizeY / 2, WorldLocation, WorldDirection);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (WorldDirection * 80000.0f);

	//타격 판정을 Host에서 실행
	C2S_OnShot(TraceStart, TraceEnd);

	//카메라 흔들기 + 총구 반동
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(UShootCameraShake::StaticClass());

	FRotator CurrentRotation = GetControlRotation();
	CurrentRotation.Pitch += 1.0f;
	CurrentRotation.Yaw += FMath::RandRange(-0.5f, 0.5f);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(CurrentRotation);

	if (bIsFire)
	{
		GetWorldTimerManager().SetTimer(ShootTimeHandle, this, &ABattleCharacter::OnShot, 0.2f);
	}
}

bool ABattleCharacter::C2S_OnShot_Validate(FVector TraceStart, FVector TraceEnd)
{
	return true;
}


void ABattleCharacter::C2S_OnShot_Implementation(FVector TraceStart, FVector TraceEnd)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> IgnoreObjects;
	FHitResult OutHit;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	IgnoreObjects.Add(this);


	//광선 추적 시작
	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
		TraceStart,
		TraceEnd,
		ObjectTypes,
		false,
		IgnoreObjects,
		EDrawDebugTrace::None,
		OutHit,
		true,
		FLinearColor::Blue,
		FLinearColor::Red,
		5.0f);

	//Muzzle 플래시랑, 총소리
	S2A_SpawnMuzzleAndSound();

	if (Result)
	{
		TraceStart = Weapon->GetSocketTransform(TEXT("MuzzleFlash")).GetLocation();
		FVector Dir = OutHit.Location - TraceStart;
		TraceEnd = TraceStart + (Dir * 2.0f);

		bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), TraceStart, TraceEnd,	ObjectTypes, false,	IgnoreObjects, EDrawDebugTrace::None, OutHit, true, FLinearColor::Green, FLinearColor::Green, 5.0f);
		if (Result)
		{
			UGameplayStatics::ApplyPointDamage(OutHit.GetActor(),
				30.0f,
				TraceEnd - TraceStart,
				OutHit,
				UGameplayStatics::GetPlayerController(GetWorld(), 0),
				this,
				UBulletDamageType::StaticClass()
			);

			S2A_SpawnHitEffect(OutHit);


			NoiseEmitter->MakeNoise(this, 1.0f, OutHit.Location);
			NoiseEmitter->NoiseLifetime = 0.2f;
		}
	}
}

void ABattleCharacter::S2A_SpawnMuzzleAndSound_Implementation()
{
	UGameplayStatics::SpawnEmitterAttached(Explosion, Weapon, FName(TEXT("MuzzleFlash")));

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExplosionSound, Weapon->GetSocketTransform(TEXT("MuzzleFlash")).GetLocation());
}

void ABattleCharacter::S2A_SpawnHitEffect_Implementation(const FHitResult& OutHit)
{
	APawn* Pawn = Cast<APawn>(OutHit.GetActor());
	if (Pawn)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodEffect, OutHit.Location, FRotator::ZeroRotator);
	}
	else
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, OutHit.Location, FRotator::ZeroRotator);
		UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletDecal, FVector(5, 5, 5), OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation(), 60.0f);
	}
}

float ABattleCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (!HasAuthority())
	{
		return 0;
	}

	if (CurrentHP <= 0)
	{
		return 0;
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent* RadialDamageEvent = (FRadialDamageEvent*)(&DamageEvent);
	}
	else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);

		if (PointDamageEvent->HitInfo.BoneName.Compare(FName(TEXT("head"))) == 0)
		{
			UE_LOG(LogClass, Warning, TEXT("Head Shot"));
			CurrentHP = 0;
		}
		else
		{
			UE_LOG(LogClass, Warning, TEXT("Shot"));
			CurrentHP -= DamageAmount;
		}
	}
	else if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;
	}

	//맞은애 PC
	ABattlePC* DamagedPC = Cast<ABattlePC>(GetController());
	//때린애 PC
	ABattlePC* AttackerPC = Cast<ABattlePC>(EventInstigator);
	if (DamagedPC)
	{
		//맞은애 PC의 로컬, Host -> Client로 호출
		DamagedPC->S2C_PlayDamageEffect();
	}


	if (CurrentHP <= 0)
	{
		DamagedPC->bIsPawnDead = true;
		if (AttackerPC && DamagedPC)
		{
			CurrentHP = 0;

			if (DamagedPC && AttackerPC)
			{
				DamagedPC->bIsPawnDead = true;

				ABattleGS* GS = Cast<ABattleGS>(UGameplayStatics::GetGameState(GetWorld()));
				if (GS)
				{
					FString InfoMessage = FString::Printf(TEXT("%s가 %s를 죽였습니다."), *AttackerPC->UserID, *DamagedPC->UserID);
					GS->InfoMessage = InfoMessage;
					GS->OnRep_InfoMessage();

					//죽은 플레이어 결과 화면 호출, Client으로 
					DamagedPC->S2C_ConclusionDie(GS->AliveCount, GS->TotalCount);
				}

				ABattleGM* GM = Cast<ABattleGM>(UGameplayStatics::GetGameMode(GetWorld()));
				if (GM) //Host
				{
					GM->CheckAliveCount();
					GM->CheckEndGame();
				}
			}
		}
		S2A_DeadProcess();
	}

	return DamageAmount;
}

void ABattleCharacter::S2A_DeadProcess_Implementation()
{
	CurrentHP = 0;
	GetMesh()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisableInput(Cast<ABattlePC>(GetController()));

	PlayAnimMontage(DeadAnimation);
}

void ABattleCharacter::S2A_SetMaxWalkSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}


void ABattleCharacter::AddPickupItemList(AMasterItem * Item)
{
	if (IsLocallyControlled())
	{

		if (Item && !Item->IsPendingKill())
		{
			CanPickupList.Add(Item);
		}

		ViewItemTooltip();
	}
}

void ABattleCharacter::RemovePickupItemList(AMasterItem * Item)
{
	if (IsLocallyControlled())
	{
		if (Item)
		{
			CanPickupList.Remove(Item);
		}

		ViewItemTooltip();
	}
}

void ABattleCharacter::ViewItemTooltip()
{
	ABattlePC* PC = Cast<ABattlePC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!PC)
	{
		return;
	}

	if (CanPickupList.Num() == 0)
	{
		PC->ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	//젤 가까운 아이템 가져오기
	AMasterItem* ClosestItem = GetClosestItem();
	if (ClosestItem)
	{
		PC->ItemTooltip->ItemName->SetText(FText::FromString(ClosestItem->ItemDataTable->GetItemData(ClosestItem->ItemIndex).ItemName));
		PC->ItemTooltip->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PC->ItemTooltip->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ABattleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleCharacter, bIsIronsight);
	DOREPLIFETIME(ABattleCharacter, bIsSprint);
	DOREPLIFETIME(ABattleCharacter, bIsProne);
	DOREPLIFETIME(ABattleCharacter, bIsProning);
	DOREPLIFETIME(ABattleCharacter, bIsFire);
	DOREPLIFETIME(ABattleCharacter, CurrentHP);
	DOREPLIFETIME(ABattleCharacter, MaxHP);
}
