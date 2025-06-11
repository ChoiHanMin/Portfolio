// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalMonster.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Monster/AIController/NormalMonsterAIController.h"
#include "Monster/MonsterHPWidget.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/DamageEvents.h" 
#include "Ingame/PortfolioCharacter.h"
#include "Ingame/InGameGameModeBase.h"
#include "Ingame/Item/MasterItem.h"

// Sets default values
ANormalMonster::ANormalMonster()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_NormalMonster(TEXT("/Script/Engine.SkeletalMesh'/Game/PolyArtWolf/Meshes/SK_Wolf.SK_Wolf'"));
	if (SK_NormalMonster.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_NormalMonster.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88.0f), FRotator(0, -90, 0));

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 300.0f;
	PawnSensing->SetPeripheralVisionAngle(60.0);

	static ConstructorHelpers::FClassFinder<UAnimInstance> Anim_Class(TEXT("/Script/Engine.AnimBlueprint'/Game/Monster/NormalMonster/NormalMonsterAnim_BP.NormalMonsterAnim_BP_C'"));
	if (Anim_Class.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(Anim_Class.Class);
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_Monster(TEXT("/Script/AIModule.BehaviorTree'/Game/Monster/NormalMonster/AI/NormalMonsterBT.NormalMonsterBT'"));
	if (BT_Monster.Succeeded())
	{
		BehaviorTree = BT_Monster.Object;
	}
	AttackCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftWeaponColli"));
	AttackCollision->SetupAttachment(GetMesh(), TEXT("Tongue_Sock"));
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_HitEffect(TEXT("/Script/Engine.ParticleSystem'/Game/Monster/NormalMonster/P_DeadEffect.P_DeadEffect'"));
	if (P_HitEffect.Succeeded())
	{
		DeadEffect = P_HitEffect.Object;
	}

	static ConstructorHelpers::FClassFinder<UMonsterHPWidget> Widget_Class(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Monster/NormalMonster/Widget/MonsterHPWidget_BP.MonsterHPWidget_BP_C'"));
	if (Widget_Class.Succeeded())
	{
		Widget->SetWidgetClass(Widget_Class.Class);
	}

	CurrentState = ENormalMonsterState::Normal;
	CurrentAnimState = ENormalMonsterAnimState::Idle;

	Tags.Add(FName(TEXT("Monster")));
}

// Called when the game starts or when spawned
void ANormalMonster::BeginPlay()
{
	Super::BeginPlay();
	DynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterial(0));
	CurrentHP = MaxHP;

	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ANormalMonster::NormalAttack);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &ANormalMonster::OnSeePawn);
	}

	UMonsterHPWidget* HPBarWidget = Cast<UMonsterHPWidget>(Widget->GetUserWidgetObject());
	if (HPBarWidget)
	{
		HPBarWidget->Percent = CurrentHP / MaxHP;
	}

	APortfolioCharacter* Player = Cast<APortfolioCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector MonsterLocation = GetActorLocation();
		SetActorRotation((PlayerLocation - MonsterLocation).Rotation());
	}
}

// Called every frame
void ANormalMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector CameraLocation;
	FRotator CameraRotation;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Dir = CameraLocation - Widget->GetComponentLocation();
	Widget->SetWorldRotation(Dir.Rotation());
}

void ANormalMonster::UpdateDissolve()
{
	DissolveTimeElapsed += 0.02f;
	float Progress = FMath::Clamp(DissolveTimeElapsed / DissolveDuration, 0.0f, 1.0f);

	DynamicMaterial->SetScalarParameterValue(TEXT("Progress"), Progress);

	if (Progress >= 1.0f)
	{
		GetWorldTimerManager().ClearTimer(DissolveTimerHandle);
	}
}

void ANormalMonster::StartDissolve(float Duration)
{
	UE_LOG(LogTemp, Warning, TEXT("Start Dissolve"));

	if (!DynamicMaterial) return;

	DissolveDuration = Duration;
	DissolveTimeElapsed = 0.0f;

	GetWorldTimerManager().SetTimer(
		DissolveTimerHandle,
		this,
		&ANormalMonster::UpdateDissolve,
		0.02f,
		true
	);
}

// Called to bind functionality to input
void ANormalMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANormalMonster::OnSeePawn(APawn* Pawn)
{
	if (CurrentState == ENormalMonsterState::Dead)
	{
		return;
	}

	APortfolioCharacter* Player = Cast<APortfolioCharacter>(Pawn);

	if (Player && Player->CurrentHP > 0 && CurrentState == ENormalMonsterState::Normal)
	{
		ANormalMonsterAIController* AIC = Cast<ANormalMonsterAIController>(GetController());
		if (AIC)
		{
			CurrentState = ENormalMonsterState::Chase;
			CurrentAnimState = ENormalMonsterAnimState::Run;

			AIC->BBComponent->SetValueAsObject(FName(TEXT("Target")), Player);
			AIC->BBComponent->SetValueAsEnum(FName(TEXT("CurrentState")), (uint8)CurrentState);
		}
	}
}

float ANormalMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0)
	{
		return 0;
	}

	if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;
		UE_LOG(LogClass, Warning, TEXT("CurrentHP : %f"), CurrentHP);

		if (CurrentHP <= 0)
		{
			CurrentState = ENormalMonsterState::Dead;
			CurrentAnimState = ENormalMonsterAnimState::Death;

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeadEffect, GetActorLocation(), FRotator::ZeroRotator);
			AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PawnSensing->Activate(false);
			Widget->SetVisibility(false);
			StartDissolve(3.0f);

			AInGameGameModeBase* GameMode = Cast<AInGameGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
			if (GameMode)
			{
				GameMode->MonsterDead();
			}

			SpawnItem();

		}

		UMonsterHPWidget* HPBarWidget = Cast<UMonsterHPWidget>(Widget->GetUserWidgetObject());
		if (HPBarWidget)
		{
			HPBarWidget->Percent = CurrentHP / MaxHP;
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

void ANormalMonster::SpawnItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Item"));

	TSubclassOf<AMasterItem> WeaponClass = TSoftClassPtr<AMasterItem>(FSoftClassPath("/Script/Engine.Blueprint'/Game/InGame/Items/MasterItem_BP.MasterItem_BP_C'")).LoadSynchronous();
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load WeaponClass"));
		return;
	}
	GetWorld()->SpawnActor<AMasterItem>(WeaponClass, GetActorLocation() - FVector(0, 0, 50), FRotator::ZeroRotator)->SetActorScale3D(FVector::OneVector * 4.f);

	//GetWorld()->SpawnActor<AMasterItem>(DropItem, GetActorLocation() - FVector(0, 0, 50), FRotator::ZeroRotator)->SetActorScale3D(FVector::OneVector * 4.f);

}

void ANormalMonster::NormalAttack(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Monster Normal Attack"));
			UGameplayStatics::ApplyDamage(OtherActor, 10.0f, GetController(), this, UDamageType::StaticClass());
		}
	}
}

void ANormalMonster::SetAttackColision(bool bIsOn)
{

	if (bIsOn)
	{
		AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}