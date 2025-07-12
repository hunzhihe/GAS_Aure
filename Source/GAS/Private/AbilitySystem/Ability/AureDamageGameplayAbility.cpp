// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


void UAureDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	//创建GE
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	//通过SetByCaller设置属性伤害
	for(auto Pair : DamageTypes)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaleDamage);
	}
	//将GE应用给目标
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));

}

FDamageEffectParams UAureDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector KnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	// 初始化伤害效果参数
	FDamageEffectParams Params;
	// 设置世界上下文对象，用于获取玩家角色
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	// 设置伤害游戏玩法效果类
	Params.DamageGameplayEffectClass = DamageEffectClass;
	// 设置来源能力系统组件，用于处理伤害效果
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	// 设置目标能力系统组件，用于应用伤害效果
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	// 遍历伤害类型，计算并设置伤害类型和对应的伤害值
	for(auto& Pair : DamageTypes)
	{
	    const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel()); 
	    Params.DamageTypes.Add(Pair.Key, ScaledDamage);
	}
	// 设置能力等级
	Params.AbilityLevel = GetAbilityLevel();
	// 设置减益伤害类型
	Params.DebuffDamageType = DebuffDamageType;
	// 设置减益概率
	Params.DebuffChance = DebuffChance;
	// 设置减益伤害值
	Params.DebuffDamage = DebuffDamage;
	// 设置减益持续时间
	Params.DebuffDuration = DebuffDuration;
	// 设置减益频率
	Params.DebuffFrequency = DebuffFrequency;
	// 设置死亡冲量大小
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	// 设置击退力大小
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	// 设置击退概率
	Params.KnockbackChance = KnockbackChance;
	
	// 如果目标角色有效，则计算伤害方向
	if (IsValid(TargetActor))
	{
	    FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
	    // 如果覆盖俯仰角，则设置俯仰角
	    if (bOverridePitch)
	    {
	        Rotation.Pitch = PitchOverride;
	    }
	    const FVector ToTarget = Rotation.Vector();
	    // 如果不覆盖击退方向，则根据伤害方向计算击退力
	    if (!bOverrideKnockbackDirection)
	    {
	        Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
	    }
	    // 如果不覆盖死亡冲量方向，则根据伤害方向计算死亡冲量
	    if (!bOverrideDeathImpulse)
	    {
	        Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
	    }
	}
	
	// 如果覆盖击退方向，则设置自定义击退方向
	if (bOverrideKnockbackDirection)
	{
	    KnockbackDirectionOverride.Normalize();
	    Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;
	    // 如果覆盖俯仰角，则调整击退力的俯仰角
	    if (bOverridePitch)
	    {
	        FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
	        KnockbackRotation.Pitch = PitchOverride;
	        Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
	    }
	}
	
	// 如果覆盖死亡冲量方向，则设置自定义死亡冲量方向
	if (bOverrideDeathImpulse)
	{
	    DeathImpulseDirectionOverride.Normalize();
	    Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
	    // 如果覆盖俯仰角，则调整死亡冲量的俯仰角
	    if (bOverridePitch)
	    {
	        FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
	        DeathImpulseRotation.Pitch = PitchOverride;
	        Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
	    }
	}
	
	// 如果是径向伤害，则设置径向伤害相关参数
	if (bIsRadialDamage)
	{
	    Params.bIsRadialDamage = bIsRadialDamage;
	    Params.RadialDamageOrigin = InRadialDamageOrigin;
	    Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
	    Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	
	// 返回初始化后的伤害效果参数
	return Params;
}

FTaggedMontage UAureDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages)
{
	if(TaggedMontages.Num() > 0)
	{
		const int32 RandomNum = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[RandomNum];
	}
	return FTaggedMontage();
}

float UAureDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType)
{
	checkf(DamageTypes.Contains(DamageType),TEXT("技能 [%s] 没有包含 [%s] 类型的伤害"),
		*GetNameSafe(this), *DamageType.ToString());
	//根据等级获取技能伤害
	return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}


