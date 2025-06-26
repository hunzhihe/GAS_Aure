// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AureAbilitySystemGlobals.h"

#include "AureAbilityTypes.h"

FGameplayEffectContext* UAureAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAureGameplayEffectContext();
}
