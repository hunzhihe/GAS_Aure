// Fill out your copyright` notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AureAbilitySystemComponent.generated.h"





DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)

//技能初始化应用后的回调委托
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven,UAureAbilitySystemComponent*)
//单播委托，只能绑定一个回调函数
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&)
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

	//网络复制，技能初始化
	virtual void OnRep_ActivateAbilities() override;

	FEffectAssetTags EffectAssetTags;

	//技能初始化应用后的回调委托
	FAbilitiesGiven AbilitiesGivenDelegate;


	//添加角色技能
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	//添加角色被动技能
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	//用来确认当运行时，是技能的初始化先完成，还是UI的初始化先完成
	bool bStartupAbilitiesGiven = false;


	

	//技能输入
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);


	/**
	 * 从能力规格中获取能力标签
	 * 
	 * @param AbilitySpec 能力规格的引用，包含能力的相关信息
	 * @return 返回对应的能力标签
	 */
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
	/**
	 * 从能力规格中获取输入标签
	 * 
	 * @param AbilitySpec 能力规格的引用，包含能力的相关信息
	 * @return 返回对应的输入标签
	 */
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
protected:                      
	//委托触发的回调函数
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
};
