// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalMonster.h"
#include "Character/MyIngameGE.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Monster/AIController/NormalMonsterAIController.h"
#include "Monster/MonsterHPWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/DamageEvents.h" 
#include "Ingame/PortfolioCharacter.h"
#include "Ingame/InGamePlayerController.h"
#include "Ingame/InGameGameModeBase.h"
#include "Ingame/Item/MasterItem.h"
#include "Engine/EngineTypes.h"      // FOverlapResult 정의
#include "CollisionQueryParams.h"    // Overlap/Trace 관련
#include "CollisionShape.h"          // FCollisionShape
#include "Engine/World.h"            // GetWorld(), OverlapMulti 등

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

	//AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ANormalMonster::NormalAttack);

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
		UE_LOG(LogTemp, Warning, TEXT("Monster is dead, cannot chase player."));
		return;
	}

	APortfolioCharacter* Player = Cast<APortfolioCharacter>(Pawn);
	
	if (Player && Player->GetCurrentHP()> 0 && CurrentState == ENormalMonsterState::Normal)
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

	//DrawDebugCapsule(GetWorld(), GetActorLocation(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Green, false, 2.0f);
	if (CurrentHP <= 0)
	{
		return 0;
	}

	if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;
		UE_LOG(LogClass, Warning, TEXT("Monster CurrentHP : %f"), CurrentHP);

		if (CurrentHP <= 0)
		{
			CurrentState = ENormalMonsterState::Dead;
			CurrentAnimState = ENormalMonsterAnimState::Death;

			ANormalMonsterAIController* AIC = Cast<ANormalMonsterAIController>(GetController());
			if(AIC)
			{
				AIC->BBComponent->SetValueAsEnum(FName(TEXT("CurrentState")), (uint8)CurrentState);
			}

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

void ANormalMonster::NormalAttack(AActor* Attacker, AActor* TargetActor, float DamageAmount)
{
	AInGamePlayerController* PC = Cast<AInGamePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	APortfolioCharacter* TargetChar = Cast<APortfolioCharacter>(TargetActor);
	if (!TargetChar) return;

	UAbilitySystemComponent* TargetASC = TargetChar->GetAbilitySystemComponent();
	if (!TargetASC) return;

	// 타겟 ASC의 EffectContext 생성 — 출처 정보로 공격자 객체를 넣어줄 수 있음
	FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
	// 누가 때렸는지 남겨두기
	Context.AddSourceObject(Attacker); 

	// 타겟 ASC로부터 OutgoingSpec 만들기 (GE_Damage는 미리 준비된 GameplayEffect 클래스)
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(PC->DamageGE, 1.0f, Context);
	if (!SpecHandle.IsValid()) return;

	//데미지 태그에 데미지 값 설정
	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), DamageAmount);
	
	// 타겟 ASC에 적용
	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}


void ANormalMonster::SetAttackColision(bool bIsOn)
{
	// 캐릭터의 정면 부채꼴 범위내에 있는 액터 검출
	float Radius = 200.0f; // 부채꼴 반지름
	float FanAngleDegree = 60.0f; // 부채꼴의 각도
	int Segment = 16; // 부채꼴을 나눌 활꼴의 갯수
	FVector Center = GetActorLocation(); // 부채꼴의 중심 위치
	FVector Forward = GetActorForwardVector(); // 부채꼴의 전방 방향
	FColor Color = FColor::Green; // 부채꼴의 색상
	float Duration = 2.0f; // 부채꼴 그리기 지속 시간

	//시작 각도 계산
	float StartAngle = -FanAngleDegree / 2.0f;
	//다음 각도 계산
	float AngleStep = FanAngleDegree / Segment;

	FVector Up = FVector::UpVector;
	FVector PrevPoint = Center + Forward.RotateAngleAxis(StartAngle, Up) * Radius;
	
	//활꼴 갯수만큼 반복
	for (int i = 1; i <= Segment; ++i)
	{
		float Angle = StartAngle + AngleStep * i;
		FVector NextPoint = Center + Forward.RotateAngleAxis(Angle, Up) * Radius;
		
		//호 그리기
		//DrawDebugLine(GetWorld(), PrevPoint, NextPoint, Color, false, Duration, 0, 2.0f);
		PrevPoint = NextPoint;
	}
	// 중심에서 호의 양 끝점까지 선 그리기
	//DrawDebugLine(GetWorld(), Center, Center + Forward.RotateAngleAxis(StartAngle, Up) * Radius, Color, false, Duration, 0, 2.0f);
	//DrawDebugLine(GetWorld(), Center, Center + Forward.RotateAngleAxis(StartAngle + FanAngleDegree, Up) * Radius, Color, false, Duration, 0, 2.0f);

	//콜라이더 설정
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);            // 자기 자신 제외

	//검출될 오브젝트 타입 설정
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);         // 캐릭터
	
	TArray<FOverlapResult> OutOverlaps;

	//오브젝트 타입 및 콜라이더 모양에 따른 겹침 검사 수행
	GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		Center + (Up * 90),
		FQuat::Identity,
		ObjectQueryParams,                   // 원하는 채널로 변경
		FCollisionShape::MakeSphere(Radius), // 또는 MakeCapsule
		Params
	);

	//검출된 액터들 중에서 부채꼴 범위 안에 있는지 확인
	for (const FOverlapResult& Result : OutOverlaps)
	{
		AActor* Target = Result.GetActor();
		if (!Target) continue;

		FVector ToTarget = (Target->GetActorLocation() - Center).GetSafeNormal();
		float Dot = FVector::DotProduct(Forward, ToTarget);
		float DegreeBetween = FMath::RadiansToDegrees(acosf(Dot));

		if (DegreeBetween <= FanAngleDegree / 2.0f)
		{
			// 부채꼴 안에 있는 액터
			UE_LOG(LogTemp, Warning, TEXT("Name : %s"), *Target->GetName());
			if (Target->ActorHasTag("Player"))
			{
				NormalAttack(this, Target, 10.0f);
			}
		}
	}
}