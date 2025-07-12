// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AureWidgetController.h"

#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "Player/AurePlayerController.h"
#include "Player/AurePlayerState.h"

void UAureWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAureWidgetController::BroadcastInitialValues()
{
	if (!GetAureASC()->bStartupAbilitiesGiven) return;
}

void UAureWidgetController::BindCallbacksToDependencies()
{
}

void UAureWidgetController::BroadcastAbilityInfo()
{
	//获取当前技能初始化是否完成
	if (!GetAureASC()->bStartupAbilitiesGiven)
	{
		return;
	}
	//创建一个单播委托
	FForEachAbility BroadcastDelegate;

	//绑定回调匿名函数，委托广播时将会触发内部逻辑
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		//获取技能实例的标签，并通过标签获取技能数据
		FAureAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAureAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));

		//获取技能的输入标签
		Info.InputTag = UAureAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);

		//获取技能的状态标签
		Info.StatusTag = UAureAbilitySystemComponent::GetStatusFromSpec(AbilitySpec);

		AbilityInfoDelegate.Broadcast(Info);
	}
		);

	//遍历技能并触发委托回调
	GetAureASC()->ForEachAbility(BroadcastDelegate);
}

void UAureWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();

	//清除旧插槽的数据
	FAureAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	//更新新插槽的数据
	FAureAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}

void UAureWidgetController::ClearAllDelegate()
{
	AbilityInfoDelegate.Clear();
}

AAurePlayerController* UAureWidgetController::GetAurePC()
{
	if (AurePlayerController == nullptr)
	{
		AurePlayerController = Cast<AAurePlayerController>(PlayerController);
	}
	return AurePlayerController;
}

 AAurePlayerState* UAureWidgetController::GetAurePS() 
{
	if (AurePlayerState == nullptr)
	{
		AurePlayerState = Cast<AAurePlayerState>(PlayerState);
	}
	return AurePlayerState;
}

UAureAbilitySystemComponent* UAureWidgetController::GetAureASC()
{
	if (AureAbilitySystemComponent == nullptr)
	{
		AureAbilitySystemComponent = Cast<UAureAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AureAbilitySystemComponent;
}

UAureAttributeSet* UAureWidgetController::GetAureAS()
{
	if (AureAttributeSet == nullptr)
	{
		AureAttributeSet = Cast<UAureAttributeSet>(AttributeSet);
	}
	return AureAttributeSet;
}
