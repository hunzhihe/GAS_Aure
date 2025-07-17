// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AurePassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"

void UAurePassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//获取到ASC
	if (UAureAbilitySystemComponent* AureASC = Cast<UAureAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		//绑定技能取消回调
		AureASC->DeactivatePassiveAbilityDelegate.AddUObject(this, &ThisClass::ReceiveDeactivate);
	}
}

void UAurePassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	//判断技能标签容器里是否包含此标签
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
	}
}
