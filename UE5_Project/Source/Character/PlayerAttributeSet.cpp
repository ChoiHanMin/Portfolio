// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerAttributeSet.h"
#include "Ingame/PortfolioCharacter.h"
#include "Ingame/InGamePlayerState.h"
#include "Ingame/InGamePlayerController.h"
#include "GameplayEffectExtension.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	// Initialize default values for attributes
	MaxHP.SetBaseValue(100.0f);
	MaxHP.SetCurrentValue(100.0f);

	CurrentHP.SetBaseValue(100.0f);
	CurrentHP.SetCurrentValue(100.0f);

	AttackDamage.SetBaseValue(10.0f);
	AttackDamage.SetCurrentValue(10.0f);
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//태그로 데미지와 회복량을 구분
	float DamageDone = Data.EffectSpec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(TEXT("Data.Damage")),
		false, 0.f
	);

	//데미지가 0보다 클 때만 적용
	if (DamageDone > 0.f)
	{
		float NewHealth = CurrentHP.GetCurrentValue() - DamageDone;
		SetCurrentHP(FMath::Clamp(NewHealth, 0.0f, CurrentHP.GetCurrentValue()));
		UE_LOG(LogTemp, Warning, TEXT("Damage Applied: %f"), DamageDone);
		UE_LOG(LogTemp, Warning, TEXT("Currnt HP: %f"), CurrentHP.GetCurrentValue());
		if (AActor* Owner = GetOwningActor())
		{
			if (APlayerState* PS = Cast<APlayerState>(Owner))
			{
				if (APortfolioCharacter* Character = Cast<APortfolioCharacter>(PS->GetPlayerController()->GetPawn()))
				{
					Character->CharacterDamaged();
				}
			}
		}
		return;
	}


	float HealValue = Data.EffectSpec.GetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(TEXT("Data.Heal")),
		false, 0.f
	);

	//회복이 0보다 클 때만 적용
	if (HealValue > 0.f)
	{
		float NewHealth = CurrentHP.GetCurrentValue() + HealValue;

		//최대 체력 이상으로 회복 불가
		if (NewHealth >= MaxHP.GetCurrentValue())
			NewHealth = MaxHP.GetCurrentValue();

		SetCurrentHP(FMath::Clamp(NewHealth, 0.0f, MaxHP.GetCurrentValue()));
		UE_LOG(LogTemp, Warning, TEXT("Heal Applied: %f"), HealValue);
		UE_LOG(LogTemp, Warning, TEXT("Currnt HP: %f"), CurrentHP.GetCurrentValue());

		if (AActor* Owner = GetOwningActor())
		{
			if (APlayerState* PS = Cast<APlayerState>(Owner))
			{
				if (APortfolioCharacter* Character = Cast<APortfolioCharacter>(PS->GetPlayerController()->GetPawn()))
				{
					Character->CharacterHealed();
				}
			}
		}
	}

}
