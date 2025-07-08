// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

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
	GetAureASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
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

	//绑定技能点变化回调
	GetAurePS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 NewSpellPoints)
	{
		//广播现拥有的技能点
		SpellPointChanged.Broadcast(NewSpellPoints);
	});
}
