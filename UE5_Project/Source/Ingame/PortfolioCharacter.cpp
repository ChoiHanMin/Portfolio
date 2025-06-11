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
#include "Ingame/InGameGameModeBase.h"

APortfolioCharacter::APortfolioCharacter()
{

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterSkeletonMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ModularRPGHeroesPolyart/Meshes/OneMeshCharacters/ApprenticeSK.ApprenticeSK'"));
	//if (CharacterSkeletonMesh.Succeeded())
	//{
	//	//defaultMeshParams.Skeleton = CharacterSkeleton.Object;
	//	GetMesh()->SetSkeletalMesh(CharacterSkeletonMesh.Object);
	//}


	//���̷�Ż �޽� ����
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


	//����ƽ�Ž� ����
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

void APortfolioCharacter::SetWeponColision(bool bIsOn)
{
	if (bIsOn)
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APortfolioCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;

	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &APortfolioCharacter::WeaponAttack);

	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PC->SetWidgetPercent(CurrentHP / MaxHP);
}

void APortfolioCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool APortfolioCharacter::IsDead()
{
	return bIsDead;
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
	if (OtherActor && OtherActor != this) 
	{
		if (OtherActor->ActorHasTag("Monster"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon Attack"));
			UGameplayStatics::ApplyDamage(OtherActor, 40.0f, GetController(), this, UDamageType::StaticClass());
		}
	}

}

float APortfolioCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0)
	{
		return 0;
	}


	if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;
		AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PC->OnDamage.ExecuteIfBound(CurrentHP / MaxHP);

		UE_LOG(LogClass, Warning, TEXT("CurrentHP : %f"), CurrentHP);

		if (CurrentHP <= 0)
		{
			bIsDead = true;
			CurrentState = ECharacterState::Dead;
			CurrentAnimState = ECharacterAnimState::Death;
			ShieldCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			AInGameGameModeBase* GameMode = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				GameMode->PlayerDead();
			}
		}
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
	}
	else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
	}
	return 0.0f;
}