// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureGameplayAbility.h"

#include "AbilitySystem/AureAttributeSet.h"


FString UAureGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("%s, <Level>%d</>"), L"默认技能名称 - 技能名称火球术", Level);
}

FString UAureGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("下一等级：<Level>%d</> 造成更多的伤害。"), Level);
}

FString UAureGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("技能将在角色等级达到<Level>%d</>时解锁"), Level);
}

void UAureGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	//通过句柄获取到技能实例
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	//技能实例在激活状态时，触发输入事件
	if (AbilitySpec->IsActive())
	{
		//将按下事件复制到服务器和所有相关的客户端
		ActorInfo->AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Handle,ActivationInfo.GetActivationPredictionKey());
		
	}
}

void UAureGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//将释放事件复制到服务器和所有相关的客户端
	ActorInfo->AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Handle,ActivationInfo.GetActivationPredictionKey());
}

float UAureGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCast = 0.f;
	//获取到冷却GE
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		//遍历GE修改的内容
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			//判断修改的属性是否为角色蓝量属性
			if (Mod.Attribute == UAureAttributeSet::GetManaAttribute())
			{
				//通过修饰符获取到使用的FScalableFloat并计算传入等级的蓝量消耗
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCast);
				//获取到了就结束遍历
				break;
			}
		}
	}
	return ManaCast;
}

float UAureGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	//获取到技能冷却GE
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		//获取到当前冷却时间
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}
