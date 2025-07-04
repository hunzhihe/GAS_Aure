// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTasks/ListenCooldownChange.h"

#include "AbilitySystemComponent.h"

UListenCooldownChange* UListenCooldownChange::ListenCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& InCooldownTags)
{
	// 创建一个新的UListenCooldownChange对象
	UListenCooldownChange* ListenCooldownChange = NewObject<UListenCooldownChange>();
	
	// 设置对象的属性，包括能力系统组件和冷却标签
	ListenCooldownChange->ASC = AbilitySystemComponent;
	ListenCooldownChange->CooldownTags = InCooldownTags;
	
	// 检查能力系统组件的有效性和冷却标签是否有效
	if (!IsValid(AbilitySystemComponent) || InCooldownTags.IsValid() == false)
	{
	    // 如果无效，结束监听任务并返回空指针
	    ListenCooldownChange->EndTask();
	    return nullptr;
	}
	
	// 注册游戏玩法标签事件，当指定的冷却标签新增或移除时，调用CooldownTagChanged方法
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTags, EGameplayTagEventType::Type::NewOrRemoved).AddUObject(
	    ListenCooldownChange, &UListenCooldownChange::CooldownTagChanged);
	
	// 注册激活效果添加事件，当有新的游戏效果激活时，调用OnActiveEffectAdded方法
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
	    ListenCooldownChange, &UListenCooldownChange::OnActiveEffectAdded);
	
	// 返回创建的监听冷却变化对象
	return ListenCooldownChange;
	
}

void UListenCooldownChange::EndTask()
{
	// 检查ASC对象是否有效，如果不有效则直接返回
	if (!IsValid(ASC))
	{
	    return;
	}
	
	// 移除与CooldownTags相关的事件监听器，以避免内存泄漏或无效引用
	ASC->RegisterGameplayTagEvent(CooldownTags, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	
	// 设置对象状态为准备销毁
	SetReadyToDestroy();
	// 标记对象为垃圾，以便垃圾回收
	MarkAsGarbage();
}

void UListenCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount) const
{
	// 如果没有新的计数（NewCount等于0），则广播冷却结束信号，参数为0表示冷却时间为0
	if (NewCount == 0)
	{
	    CooldownEnd.Broadcast(0.f);
	}
}

void UListenCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
                                                const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle) const
{
	// 获取资产标签
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);
	
	// 获取授予权标签
	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);
	
	// 检查资产标签或授予权标签中是否包含冷却标签
	if (AssetTags.HasTagExact(CooldownTags) || GrantedTags.HasTagExact(CooldownTags))
	{
	    // 创建一个游戏效果查询，匹配拥有冷却标签的任何效果
	    FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags.GetSingleTagContainer());
	    
	    // 获取所有匹配的游戏效果的剩余时间
	    TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
	    
	    // 如果存在匹配的游戏效果
	    if (TimesRemaining.Num() > 0)
	    {
	        // 初始化剩余时间变量
	        float TimeRemaining = TimesRemaining[0];
	        
	        // 遍历所有游戏效果的剩余时间，找到最长的剩余时间
	        for (int32 i = 0; i < TimesRemaining.Num(); i++)
	        {
	            if (TimesRemaining[i] > TimeRemaining)
	            {
	                TimeRemaining = TimesRemaining[i];
	            }
	        }
	        
	        // 广播冷却开始事件，传递最长的剩余时间
	        CooldownStart.Broadcast(TimeRemaining);
	    }
	}
}
