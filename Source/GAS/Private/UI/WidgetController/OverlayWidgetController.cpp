// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AurePlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	
	OnHealthChanged.Broadcast(GetAureAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAureAS()->GetMaxHealth());
	
	OnManaChanged.Broadcast(GetAureAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAureAS()->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	// 当经验点(XP)发生变化时，注册一个回调函数
	GetAurePS()->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);
	
	// 当玩家等级发生变化时，注册一个Lambda回调函数
	GetAurePS()->OnLevelChangedDelegate.AddLambda(
	    [this](int32 NewLevel, bool bLevelUp)
	    {
	        // 当玩家等级变化时，广播该变化
	        OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
	    }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAureAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data){OnHealthChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAureAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data){OnMaxHealthChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAureAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data){OnManaChanged.Broadcast(Data.NewValue);});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAureAS()->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data){OnMaxManaChanged.Broadcast(Data.NewValue);});

	if (UAureAbilitySystemComponent* AureASC = GetAureASC())
	{
		if (AureASC->bStartupAbilitiesGiven)
		{
			//如果执行到此处时，技能的初始化工作已经完成，则直接调用初始化回调
			OnInitializeStartupAbilities(AureASC);
		}
		else
		{
			//如果执行到此处，技能初始化还未未完成，将通过绑定委托，监听广播的形式触发初始化完成回调
			AureASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
		}
		
		AureASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& Container)
		{

			for(const FGameplayTag& Tag : Container)
			{
				//将tag广播给Widget Controller
				// const FString Msg = FString::Printf(TEXT("GE Tag in Widget Controller: %s"), *Tag.ToString()); //获取Asset Tag
				// GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, Msg); //打印到屏幕上 -1 不会被覆盖

				//过滤掉非Message Tag
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if(Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAureAbilitySystemComponent* AureASC) const
{
	//获取当前技能初始化是否完成
	if (!AureASC->bStartupAbilitiesGiven)
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

		AbilityInfoDelegate.Broadcast(Info);
	}
		);

	//遍历技能并触发委托回调
	AureAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) 
{
   // 获取玩家升级信息对象
   ULevelUpInfo* LevelUpInfo = GetAurePS()->LevelUpInfo;
   // 确保LevelUpInfo不为空，否则抛出异常
   checkf(LevelUpInfo, TEXT("无法查询到等级相关数据，请查看PlayerState是否设置了对应的数据"));

   // 根据新经验值找到对应的等级
   const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
   // 获取最大等级
   const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

   // 检查当前等级是否在有效范围内
   if (Level <= MaxLevel && Level > 0)
   {
    // 获取当前等级的升级所需经验值
    const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
    // 获取前一级的升级所需经验值
    const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

    // 计算当前等级的经验值区间
    const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
    // 计算玩家在当前等级的经验值
    const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

    // 计算经验值百分比
    const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

    // 广播经验值百分比变化事件
    OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
    }

}
