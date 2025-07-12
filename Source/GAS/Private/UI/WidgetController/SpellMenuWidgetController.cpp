// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Player/AurePlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	//广播所拥有的技能点
	SpellPointChanged.Broadcast(GetAurePS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	
	// 当Aure的能力状态发生变化时，执行以下Lambda函数
	GetAureASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
	{
       //技能状态修改，修改技能的升降级按钮的状态
       if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
       {
	       SelectedAbility.AbilityStatus = StatusTag;
       	   SelectedAbility.Level = Level;

       	   //广播升降级按钮状态
       	   BroadcastSpellGlobeSelected();
       }
		
	    // 如果AbilityInfo对象存在，则查找与AbilityTag对应的能力信息
	    if (AbilityInfo)
	    {
	        FAureAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	        // 更新能力的状态标签
	        Info.StatusTag = StatusTag;
	        // 广播更新后的能力信息
	        AbilityInfoDelegate.Broadcast(Info);
	    }
	});

    //监听技能装配的回调
	GetAureASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);
	
	//绑定技能点变化回调
	GetAurePS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 NewSpellPoints)
	{
		//广播拥有的技能点
		SpellPointChanged.Broadcast(NewSpellPoints);

		CurrentSpellPoints = NewSpellPoints;

		//广播升降级按钮状态
		BroadcastSpellGlobeSelected();
	});
}

void USpellMenuWidgetController::BroadcastSpellGlobeSelected() 
{
	//技能是否可以升级
	bool bEnableSpendPoints = false;
	//技能是否可以装配
	bool bEnableEquip = false;
	//技能是否可以降级
	bool bEnableDemotion = false;

	//获取结果
	ShouldEnableButtons(SelectedAbility.AbilityStatus,
		CurrentSpellPoints>0,
		bEnableSpendPoints,bEnableEquip,bEnableDemotion);
	//广播状态
	SpellGlobeSelectedSignature.Broadcast(bEnableSpendPoints,bEnableEquip,bEnableDemotion,SelectedAbility.Level);


	//广播技能描述
	FString Description;
	FString NextLevelDescription;
	GetAureASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
	SpellDescriptionSignature.Broadcast(Description, NextLevelDescription);
}

FGameplayTag USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	// 获取游戏玩法标签实例
    const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();

    // 获取当前的法术点数
    const int32 SpellPoints = GetAurePS()->GetSpellPoints();

    // 定义能力状态标签
    FGameplayTag AbilityStatus;


    // 检查能力标签是否有效
    const bool bTagValid = AbilityTag.IsValid();

    // 检查能力标签是否为无效名称
	const bool bTagName = AbilityTag.MatchesTag(GameplayTags.Abilities_None);

    // 从能力标签获取能力规格
	const FGameplayAbilitySpec* AbilitySpec = GetAureASC()->GetSpecFromAbilityTag(AbilityTag);

    // 检查能力规格是否有效
    const bool bSpecValid = AbilitySpec != nullptr;

    // 根据标签有效性设置能力状态
    if (!bTagValid || bTagName || !bSpecValid)
    {
       AbilityStatus = GameplayTags.Abilities_Status_Locked;
    }
    else
    {
    // 获取能力规格的状态
    AbilityStatus = GetAureASC()->GetStatusFromSpec(*AbilitySpec);
    	SelectedAbility.Level = AbilitySpec->Level;
    }

   // 初始化按钮使能标志
    SelectedAbility.Ability = AbilityTag;
	SelectedAbility.AbilityStatus = AbilityStatus;
	CurrentSpellPoints = SpellPoints;

	
   // 根据能力状态和法术点数判断是否使能相关按钮
   BroadcastSpellGlobeSelected();

   // 返回能力状态
   return AbilityStatus;

}

void USpellMenuWidgetController::SpendPointButtonPressed(const FGameplayTag& AbilityTag)
{
	if (GetAureASC())
	{
		//调用ASC等级提升函数
		GetAureASC()->ServerSpendSpellPoint(AbilityTag);
	}
}

void USpellMenuWidgetController::DemotionPointButtonPressed(const FGameplayTag& AbilityTag)
{
	if (GetAureASC())
	{
		//调用ASC降低技能函数
		GetAureASC()->ServerDemotionSpellPoint(AbilityTag);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
	SelectedAbility.Ability = GameplayTags.Abilities_None;
	SelectedAbility.AbilityStatus = GameplayTags.Abilities_Status_Locked;
	SelectedAbility.Level = 0;

	SpellDescriptionSignature.Broadcast(FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();

	//获取技能的类型
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;

	// 检查技能类型是否匹配
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	//获取技能的驶入标签
	FGameplayAbilitySpec* AbilitySpec = GetAureASC()->GetSpecFromAbilityTag(SelectedAbility.Ability);
	const FGameplayTag& SelectedAbilityInputTag = GetAureASC()->GetInputTagFromSpec(*AbilitySpec);
	//如果当前技能输入和插槽标签相同，证明已经装配，不需要再处理
	if (SelectedAbilityInputTag.MatchesTagExact(SlotTag)) return;

	
	//调用装配技能函数，进行处理
	GetAureASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::ClearAllDelegate()
{
	Super::ClearAllDelegate();
	SpellPointChanged.Clear();
	SpellGlobeSelectedSignature.Clear();
	SpellDescriptionSignature.Clear();
	SelectedAbility = {FAureGameplayTags::Get().Abilities_None,FAureGameplayTags::Get().Abilities_Status_Locked};
	
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool HasSpellPoints,bool& bShouldEnableSpellPoints,
                                                     bool& bShouldEnableEquip, bool& bShouldDemotionPoints)
{
	// 获取游戏玩法标签实例，用于后续的能力状态匹配
	const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
	
	// 根据能力状态标签匹配，决定是否应该启用技能点、装备和降级点
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
	    // 如果能力状态为已装备，则根据是否有技能点来决定是否启用技能点，同时启用装备和降级点
	    bShouldEnableSpellPoints = HasSpellPoints;
	    bShouldEnableEquip = true;
	    bShouldDemotionPoints = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
	    // 如果能力状态为符合条件但未装备，则根据是否有技能点来决定是否启用技能点，但不启用装备和降级点
	    bShouldEnableSpellPoints = HasSpellPoints;
	    bShouldEnableEquip = false;
	    bShouldDemotionPoints = false;
	}
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
	    // 如果能力状态为已解锁，则根据是否有技能点来决定是否启用技能点，同时启用装备和降级点
	    bShouldEnableSpellPoints = HasSpellPoints;
	    bShouldEnableEquip = true;
	    bShouldDemotionPoints = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
	    // 如果能力状态为锁定，则不启用技能点、装备和降级点
	    bShouldEnableSpellPoints = false;
	    bShouldEnableEquip = false;
	    bShouldDemotionPoints = false;
	}
}
