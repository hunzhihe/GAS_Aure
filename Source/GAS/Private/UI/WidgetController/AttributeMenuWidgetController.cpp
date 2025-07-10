// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystemComponent.h"
#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AurePlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// 检查AttributeInfo的有效性或进行初始化
    check(AttributeInfo);
 
	// 遍历从Tags到Attributes的映射，为每个Attribute注册值变化监听器
    for (auto& Pair : GetAureAS()->TagsToAttributes)
    {
      // 为当前Attribute的值变化事件添加一个Lambda处理函数
       AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
    [this, Pair](const FOnAttributeChangeData& Data)
       {
        // 当Attribute值变化时，广播相关信息
        BroadcastAttributeInfo(Pair.Key, Pair.Value());
       }
       );
    }

	// 为玩家属性点数变化事件添加一个Lambda处理函数
	GetAurePS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);

}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
   // 将AttributeSet转换为UAureAttributeSet类型，并进行类型检查
    UAureAttributeSet* AS = CastChecked<UAureAttributeSet>(AttributeSet);
   // 确保AttributeInfo有效，进行安全性检查
    check(AttributeInfo);

   // 遍历UAureAttributeSet中的TagsToAttributes映射
     for (auto& Pair : AS->TagsToAttributes)
     {
       // 对每一对标签和属性值，广播属性信息
       BroadcastAttributeInfo(Pair.Key, Pair.Value());
     }

	// 将玩家属性点数初始化并广播
	AttributePointsChangedDelegate.Broadcast(GetAurePS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	// 将玩家属性点数减少并升级属性
	UAureAbilitySystemComponent* AureASC = CastChecked<UAureAbilitySystemComponent>(AbilitySystemComponent);
	AureASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::ClearAllDelegate()
{
	Super::ClearAllDelegate();

	AttributeInfoDelegate.Clear();
	AttributePointsChangedDelegate.Clear();
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute) const
{
	FAureAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
