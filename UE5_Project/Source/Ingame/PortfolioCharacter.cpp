// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ingame/PortfolioCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Ingame/InGamePlayerController.h"
#include "PlayableGameInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Engine/DamageEvents.h" 
#include "Ingame/InGamePlayerState.h"
#include "Ingame/InGameGameModeBase.h"
#include "Engine/EngineTypes.h"      // FOverlapResult 정의
#include "CollisionQueryParams.h"    // Overlap/Trace 관련
#include "CollisionShape.h"          // FCollisionShape
#include "Engine/World.h"            // GetWorld(), OverlapMulti 등

APortfolioCharacter::APortfolioCharacter()
{

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	//AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//AbilitySystemComponent->SetIsReplicated(true);
	//AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	//
	//AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));


	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterSkeletonMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/OneMeshCharacters/ApprenticeSK.ApprenticeSK'"));
	//if (CharacterSkeletonMesh.Succeeded())
	//{
	//	//defaultMeshParams.Skeleton = CharacterSkeleton.Object;
	//	GetMesh()->SetSkeletalMesh(CharacterSkeletonMesh.Object);
	//}


	//static ConstructorHelpers::FObjectFinder<USkeleton> CharacterSkeleton(TEXT("/Script/Engine.Skeleton'/Game/ModularRPGHeroesPolyart/Meshes/UE4_Mannequin_Skeleton.UE4_Mannequin_Skeleton'"));
	//if (CharacterSkeleton.Succeeded())
	//{
	//	defaultMeshParams.Skeleton = CharacterSkeleton.Object;
	//}

	//for (const FString& Path : MeshPaths)
	//{
	//	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshs(*Path);
	//	if (SkeletalMeshs.Succeeded())
	//	{
	//		defaultMeshParams.MeshesToMerge.Add(SkeletalMeshs.Object);
	//	}
	//}

	//defaultMeshParams.bNeedsCpuAccess = false;
	//defaultMeshParams.bSkeletonBefore = false;
	//defaultMeshParams.StripTopLODS = 0;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	Weapons = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapons"));
	Weapons->SetupAttachment(RootComponent);

	Bow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bow"));
	Bow->SetupAttachment(Weapons);

	BowArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BowArrow"));
	BowArrow->SetupAttachment(Weapons);

	LeftWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWeapon"));
	LeftWeapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	LeftWeapon->SetupAttachment(Weapons);

	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponColli"));
	WeaponCollision->SetupAttachment(LeftWeapon);
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Backpack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Backpack"));
	Backpack->SetupAttachment(Weapons);

	RightWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWeapon"));
	RightWeapon->SetupAttachment(Weapons);

	Shield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield"));
	Shield->SetupAttachment(GetMesh(), TEXT("LeftWeaponShield"));
	Shield->SetupAttachment(Weapons);

	ShieldCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldColli"));
	ShieldCollision->SetupAttachment(Shield);
	ShieldCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetWeaponMesh();

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Tags.Add(FName(TEXT("Player")));
}

UAbilitySystemComponent* APortfolioCharacter::GetAbilitySystemComponent()
{
	return AbilitySystemComponent;
}

void APortfolioCharacter::HandleHPChanged(const FOnAttributeChangeData& Data)
{
	float NewHP = Data.NewValue;
	float OldHP = Data.OldValue;

	float DeltaValue = NewHP - OldHP;
	PlayerHPChanged(DeltaValue, FGameplayTagContainer());
}

void APortfolioCharacter::SetWeponColision(bool bIsOn)
{
	if (bIsOn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Collision on"));
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Collision off"));
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APortfolioCharacter::BeginPlay()
{
	Super::BeginPlay();


	AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
	if (PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}

	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &APortfolioCharacter::WeaponAttack);

	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PC->SetWidgetHPPercent(AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute()), AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetMaxHPAttribute()));

}

void APortfolioCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool APortfolioCharacter::IsDead()
{
	return bIsDead;
}

void APortfolioCharacter::CharacterDamaged()
{
	AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PC->OnDamage.ExecuteIfBound(AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute()), AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetMaxHPAttribute()));
	UE_LOG(LogClass, Warning, TEXT("Player CurrentHP : %f"), AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute()));

	if (AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute()) <= 0)
	{
		bIsDead = true;
		CurrentState = ECharacterState::Dead;
		CurrentAnimState = ECharacterAnimState::Death;
		ShieldCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AInGameGameModeBase* GameMode = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			DisableInput(nullptr);
			GameMode->PlayerDead();
		}
	}

}

void APortfolioCharacter::CharacterHealed()
{
	AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	float currenthp = AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute());
	float maxhp = AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetMaxHPAttribute());
	UE_LOG(LogClass, Warning, TEXT("Player CurrentHP : %f"), currenthp);

	PC->OnDamage.ExecuteIfBound(currenthp, maxhp);
}

void APortfolioCharacter::Respawn()
{
	bIsDead = false;
	CurrentState = ECharacterState::Normal;
	CurrentAnimState = ECharacterAnimState::Idle;
	ShieldCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PS)
	{
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AbilitySystemComponent->SetNumericAttributeBase(PS->AttributeSet->GetCurrentHPAttribute(), AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetMaxHPAttribute()));
	}

	float currenthp = AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute());
	float maxhp = AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetMaxHPAttribute());
	PC->OnDamage.ExecuteIfBound(currenthp, maxhp);
	EnableInput(Cast<APlayerController>(GetController()));
}

float APortfolioCharacter::GetCurrentHP()
{
	AInGamePlayerState* PS = GetPlayerState<AInGamePlayerState>();
	if (PS)
	{
		return AbilitySystemComponent->GetNumericAttribute(PS->AttributeSet->GetCurrentHPAttribute());
	}
	else
	{
		return 0.0f;
	}
}

void APortfolioCharacter::SetWeaponMesh()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Weapon(TEXT("/Script/Engine.StaticMesh'/Game/ModularRPGHeroesPolyart/Meshes/Weapons/Sword02SM.Sword02SM'"));
	if (SK_Weapon.Succeeded())
	{
		LeftWeapon->SetStaticMesh(SK_Weapon.Object);
		LeftWeapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Shiled(TEXT("/Script/Engine.StaticMesh'/Game/ModularRPGHeroesPolyart/Meshes/Weapons/Shield04SM.Shield04SM'"));
	if (SK_Shiled.Succeeded())
	{
		Shield->SetStaticMesh(SK_Shiled.Object);
		Shield->SetupAttachment(GetMesh(), TEXT("LeftWeaponShield"));
	}

}

void APortfolioCharacter::WeaponAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Attack Click Click Click "));

	float Radius = 200.0f; // 팬의 반지름
	float FanAngleDegree = 60.0f; // 팬의 각도
	int Segment = 16; // 팬을 나눌 세그먼트 수
	FVector Center = GetActorLocation(); // 팬의 중심 위치
	FVector Forward = GetActorForwardVector(); // 팬의 전방 방향
	FColor Color = FColor::Red; // 팬의 색상
	float Duration = 2.0f; // 팬의 지속 시간


	float StartAngle = -FanAngleDegree / 2.0f;
	float AngleStep = FanAngleDegree / Segment;

	FVector Up = FVector::UpVector;

	FVector PrevPoint = Center + Forward.RotateAngleAxis(StartAngle, Up) * Radius;
	for (int i = 1; i <= Segment; ++i)
	{
		float Angle = StartAngle + AngleStep * i;
		FVector NextPoint = Center + Forward.RotateAngleAxis(Angle, Up) * Radius;
		DrawDebugLine(GetWorld(), PrevPoint, NextPoint, Color, false, Duration, 0, 2.0f);
		PrevPoint = NextPoint;
	}
	// 중심에서 양 끝점까지 선 그리기
	DrawDebugLine(GetWorld(), Center, Center + Forward.RotateAngleAxis(StartAngle, Up) * Radius, Color, false, Duration, 0, 2.0f);
	DrawDebugLine(GetWorld(), Center, Center + Forward.RotateAngleAxis(StartAngle + FanAngleDegree, Up) * Radius, Color, false, Duration, 0, 2.0f);

	//DrawDebugCapsule(GetWorld(), GetActorLocation(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Green, false, 2.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);            // 자기 자신 제외

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);         // 캐릭터
	//ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); // 움직이는 오브젝트 등



	TArray<FOverlapResult> OutOverlaps;

	GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		Center + (Up * 30),
		FQuat::Identity,
		ObjectQueryParams,                   // 원하는 채널로 변경
		FCollisionShape::MakeSphere(Radius), // 또는 MakeCapsule
		Params
	);

	for (const FOverlapResult& Result : OutOverlaps)
	{
		AActor* Target = Result.GetActor();
		if (!Target) continue;

		FVector ToTarget = (Target->GetActorLocation() - Center).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToTarget);
		float DegreeBetween = FMath::RadiansToDegrees(acosf(Dot));

		if (DegreeBetween <= FanAngleDegree / 2.0f)
		{
			// 부채꼴 안에 있는 액터!
			UE_LOG(LogTemp, Warning, TEXT("Name : %s"), *Target->GetName());
			if (Target->ActorHasTag("Monster"))
			{
				UGameplayStatics::ApplyDamage(Target, 10.0f, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}

/*예전 데미지 처리 방식*/
float APortfolioCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//if (CurrentHP <= 0 || DamageCauser == this)
	//{
	//	return 0;
	//}
	//
	//
	//if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("DamageCauser Name : %s"), *DamageCauser->GetName());
	//
	//
	//	CurrentHP -= DamageAmount;
	//	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//	PC->OnDamage.ExecuteIfBound(CurrentHP , MaxHP);
	//
	//	UE_LOG(LogClass, Warning, TEXT("Player CurrentHP : %f"), CurrentHP);
	//
	//	if (CurrentHP <= 0)
	//	{
	//		bIsDead = true;
	//		CurrentState = ECharacterState::Dead;
	//		CurrentAnimState = ECharacterAnimState::Death;
	//		ShieldCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//
	//		AInGameGameModeBase* GameMode = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	//		if (GameMode)
	//		{
	//			GameMode->PlayerDead();
	//		}
	//	}
	//}
	//else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	//{
	//}
	//else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	//{
	//}
	return 0.0f;
}