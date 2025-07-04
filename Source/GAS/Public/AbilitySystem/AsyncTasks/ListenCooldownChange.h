// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ListenCooldownChange.generated.h"


struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, CooldownRemaining);

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class GAS_API UListenCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	//冷却触发开始委托
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;

	//冷却触发结束委托
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

	//监听技能冷却
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UListenCooldownChange* ListenCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTags);

	//结束任务
	UFUNCTION(BlueprintCallable)
	void EndTask();


protected:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	//记录监听的冷却标签
	FGameplayTag CooldownTags;

	//监听冷却标签回调函数
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount) const;

	//监听ASC激活GE的回调
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
		const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle) const;
	
};
