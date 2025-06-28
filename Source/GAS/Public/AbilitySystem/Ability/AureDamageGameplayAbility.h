// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureAbilityTypes.h"
#include "AbilitySystem/Ability/AureGameplayAbility.h"
#include "Interaction/CombatInterface.h"
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

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector KnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f) const;

	//从角色设置的蒙太奇数组总，随机一个蒙太奇使用
	UFUNCTION(BlueprintPure)
	static FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages);
	

	
protected:

	//伤害效果类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageEffect")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	//伤害类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	//敌人受到伤害时附加负面效果的概率
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DebuffDamageType;
	

	//敌人受到伤害时附加负面效果的概率
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;

	//负面效果造成的伤害量
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;

	//负面效果的触发频率
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;

	//负面效果的持续时间
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.f;

    // DeathImpulseMagnitude定义了死亡时施加的冲量大小，仅在编辑默认设置时可配置
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float DeathImpulseMagnitude = 1000.f;
    
    // KnockbackForceMagnitude表示击退力的大小，仅在编辑默认设置时可配置
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float KnockbackForceMagnitude = 1000.f;
    
    // KnockbackChance表示击退发生的概率，仅在编辑默认设置时可配置
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float KnockbackChance = 0.f;
    
    // bIsRadialDamage指示是否使用径向（范围）伤害，仅在编辑默认设置时可配置
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    bool bIsRadialDamage = false;
    
    // RadialDamageInnerRadius定义了径向伤害的内半径，仅在编辑默认设置时可配置，且在蓝图中可读取
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
    float RadialDamageInnerRadius = 0.f;
    
    // RadialDamageOuterRadius定义了径向伤害的外半径，仅在编辑默认设置时可配置，且在蓝图中可读取
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
    float RadialDamageOuterRadius = 0.f;
};
