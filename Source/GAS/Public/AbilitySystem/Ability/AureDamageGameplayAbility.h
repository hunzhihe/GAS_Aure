// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureGameplayAbility.h"
#include "AureDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureDamageGameplayAbility : public UAureGameplayAbility
{
	GENERATED_BODY()
	public:
	// 造成伤害
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

protected:

	//伤害效果类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageEffect")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	//伤害类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
