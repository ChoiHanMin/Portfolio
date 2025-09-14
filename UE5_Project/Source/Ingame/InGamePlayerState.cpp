// Fill out your copyright notice in the Description page of Project Settings.


#include "Ingame/InGamePlayerState.h"

AInGamePlayerState::AInGamePlayerState()
{
    // ASC ����
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // AttributeSet ����
    AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AInGamePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
