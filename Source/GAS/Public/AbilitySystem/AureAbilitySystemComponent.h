// Fill out your copyright` notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AureAbilitySystemComponent.generated.h"





DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/,int32);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, const FGameplayTag& /*输入标签*/, const FGameplayTag& /*上一个输入标签*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*技能标签*/); //中止一个技能激活的回调
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivePassiveEffect, const FGameplayTag& /*被动技能标签*/, bool /*激活或取消*/); //被动技能特效监听委托，对应特效是否开启
//技能初始化应用后的回调委托
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven)
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

	//技能状态改变的回调委托
	FAbilityStatusChanged AbilityStatusChangedDelegate;

	//技能装配插槽改变的回调委托
	FAbilityEquipped AbilityEquippedDelegate;

	//取消技能激活的委托
	FDeactivatePassiveAbility DeactivatePassiveAbilityDelegate;
	
	//被动技能对应特效委托
	FActivePassiveEffect ActivatePassiveEffect;

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
	//获取技能状态（锁定，待解锁，已解锁，已装备）标签
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	// 根据能力标签获取状态
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	// 根据能力标签获取槽位
	FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag);
	// 检查槽位是否为空
	bool SlotIsEmpty(const FGameplayTag& Slot);
	// 检查能力是否具有指定的槽位
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	// 检查能力是否具有任何槽位
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	// 根据槽位获取能力规格
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	// 检查能力是否为被动能力
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	// 为能力分配槽位
	static void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	
	/**
		 * 多网络被动特效委托广播，让每个客户端都可以看到特效
		 * @param AbilityTag 被动技能标签
		 * @param bActivate 激活或者关闭
		 */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);


	
    //通过技能标签获取技能实例
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	//根据角色等级更新技能数据状态
	void UpdateAbilityStatuses(int32 Level);
	

	//升级属性
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	//服务器升级属性
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerDemotionSpellPoint(const FGameplayTag& AbilityTag);

	//通过技能标签获取技能描述
	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);


	//服务器处理技能装配，传入技能标签和装配的插槽技能标签
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	//客户端处理技能装配其
	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	//清除技能装配插槽中的技能
	static void ClearSlot(FGameplayAbilitySpec* Spec);

	//根据输入标签，清除技能装配插槽的技能
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);

	//判断当前技能实例是否处于目标技能装配插槽
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);
	
protected:                      
	//委托触发的回调函数
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
    //技能状态改变的广播函数，只在客户端广播
	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level);


	
};
