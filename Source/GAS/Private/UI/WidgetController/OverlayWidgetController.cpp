// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	
	OnHealthChanged.Broadcast(GetAureAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAureAS()->GetMaxHealth());
	
	OnManaChanged.Broadcast(GetAureAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAureAS()->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	//const UAureAttributeSet* AureAttributeSet = CastChecked<UAureAttributeSet>(AttributeSet);

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
