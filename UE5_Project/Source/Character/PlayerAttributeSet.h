// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

/**
 * 
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PORTFOLIO_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

    UPlayerAttributeSet();
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayAttributeData CurrentHP;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CurrentHP)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayAttributeData MaxHP;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHP)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayAttributeData AttackDamage;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AttackDamage)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defense)

    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
};
