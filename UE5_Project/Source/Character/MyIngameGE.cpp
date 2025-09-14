// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyIngameGE.h"
#include "Character/PlayerAttributeSet.h"
#include "GameplayEffectExtension.h"

UMyIngameGE::UMyIngameGE()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;


	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UPlayerAttributeSet::GetCurrentHPAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(0.f); // SetByCaller�� ����� ����
	Modifiers.Add(Modifier);
}
