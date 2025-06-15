// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AureAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
/**
 * 
 */
UCLASS()
class GAS_API UAureAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	//初始化GE相关委托的注册
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
protected:                      
	//委托触发的回调函数
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};
