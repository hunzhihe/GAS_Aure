// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AureAbilitySystemComponent.h"


void UAureAbilitySystemComponent::AbilityActorInfoSet()
{
	
	// 当游戏玩法效果应用于此组件时，将触发ClientEffectApplied_Implementation函数
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(
	    this, &UAureAbilitySystemComponent::ClientEffectApplied);
}

void UAureAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
    // for (FGameplayTag Tag : TagContainer)
    // {
	   //  const FString Msg = FString::Printf(TEXT("Effect Applied(GE Tag): %s"), *Tag.ToString());
    // 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);//  调试信息  -1表示不会被覆盖
    // }
	
	
}

