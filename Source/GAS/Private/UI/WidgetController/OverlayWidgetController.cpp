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
	
	GetAureASC()->EffectAssetTags.AddLambda(
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
