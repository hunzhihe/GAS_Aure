// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AureAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AureGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Ability/AureGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "GAS/AureLogChannels.h"
#include "Interaction/PlayerInterface.h"


void UAureAbilitySystemComponent::AbilityActorInfoSet()
{
	
	// 当游戏玩法效果应用于此组件时，将触发ClientEffectApplied_Implementation函数
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(
	    this, &UAureAbilitySystemComponent::ClientEffectApplied);
}

void UAureAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAureAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	// 遍历初始化能力列表，为对象赋予一系列预设的能力
	for (auto AbilityClass : StartupAbilities)
	{
		// 创建一个游戏玩法能力规范实例，指定能力的类和等级
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		// 尝试将规范中的能力转换为特定的游戏玩法能力
		if (const UAureGameplayAbility* AuraAbility = Cast<UAureGameplayAbility>(AbilitySpec.Ability))
		{
		    // 添加启动输入标签到能力规范的动态源标签中
		    AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAureGameplayTags::Get().Abilities_Status_Equipped);
			
		    // 授予该能力
		    GiveAbility(AbilitySpec);
		}
	}
	//技能初始化完成，并广播
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAureAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		GiveAbilityAndActivateOnce(AbilitySpec); //应用技能并激活一次
	}
}

void UAureAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{

}


void UAureAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// 检查输入标签是否有效，如果无效则直接返回
	if (!InputTag.IsValid()) return;
	
	// 遍历所有可激活的能力
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
	    // 检查能力规格是否具有与输入标签完全匹配的动态源标签
	    if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
	    {
	        // 当输入标签与能力的动态源标签匹配时，调用 AbilitySpecInputPressed 函数
	        AbilitySpecInputPressed(AbilitySpec);
	        
	        // 如果当前能力不处于激活状态，则尝试激活该能力
	        if (!AbilitySpec.IsActive())
	        {
	            TryActivateAbility(AbilitySpec.Handle);
	        }
	    }
	}
	
}

void UAureAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	// 检查输入标签是否有效，如果无效则直接返回
	if(!InputTag.IsValid()) return;
	
	// 遍历所有可激活的能力
	for(auto AbilitySpec : GetActivatableAbilities())
	{
	    // 检查能力规格中的动态源标签是否包含与输入标签完全匹配的标签
	    if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
	    {
	        // 如果存在匹配的标签，则调用能力规格输入释放函数
	        AbilitySpecInputReleased(AbilitySpec);
	    }
	}
}

void UAureAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	//适用域锁将此作用域this的内容锁定（无法修改），在遍历结束时解锁，保证线程安全
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//运行绑定在技能实例上的委托，如果失败返回false
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAure,Error,TEXT("在函数[%hs]运行委托失败"),__FUNCTION__)
		}
	}
}

FGameplayTag UAureAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	// 检查AbilitySpec中是否指定了能力
	if (AbilitySpec.Ability)
	{
	    // 遍历能力的标签，寻找特定的"Abilities"标签
	    for (FGameplayTag Tag : AbilitySpec.Ability.Get()->GetAssetTags())
	    {
	        // 如果标签匹配"Abilities"，则返回该标签
	        if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
	        {
	            return Tag;
	        }
	    }
	}
	// 如果没有找到匹配的标签，返回一个空的标签
	return FGameplayTag();
}

FGameplayTag UAureAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	// 遍历能力规格中的动态规格源标签
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
	    // 检查当前标签是否匹配"InputTag"游戏标签
	    if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
	    {
	        // 如果匹配，则返回该标签
	        return Tag;
	    }
	}
	// 如果没有找到匹配的标签，则返回一个空的游戏标签
	return FGameplayTag();
}

FGameplayTag UAureAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAureAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	// 使用适用域锁将此作用域this的内容锁定（无法修改），在遍历结束时解锁，保证线程安全
	FScopedAbilityListLock ActiveScopeLoc(*this);
	
	// 遍历所有可激活的能力规格
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
	    // 遍历当前能力规格的所有标签
	    for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
	    {
	        // 检查当前标签是否与指定的能力标签匹配
	        if (Tag.MatchesTag(AbilityTag))
	        {
	            // 如果匹配，则返回该能力规格的指针
	            return &AbilitySpec;
	        }
	    }
	}
	// 如果没有找到匹配的能力标签，则返回空指针
	return nullptr;
}

void UAureAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{

	// 获取技能信息
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	// 遍历角色的技能信息列表
	for (const FAureAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
	    // 如果技能标签无效，则跳过当前技能
	    if (!Info.AbilityTag.IsValid()) continue;
	    
	    // 如果玩家等级未达到技能要求的等级，则跳过当前技能
	    if (Level < Info.LevelRequirement) continue;
	
	    // 检查当前技能是否已经具有规格（Spec）信息
	    if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
	    {
	        // 创建一个新的技能规格对象，初始化技能和等级
	        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
	        
	        // 为技能规格添加动态源标签，表明该技能处于可使用状态
	        AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAureGameplayTags::Get().Abilities_Status_Eligible);
	        
	        // 授予玩家该技能
	        GiveAbility(AbilitySpec);
	        
	        // 标记技能规格为脏，以便于后续更新或同步
	        MarkAbilitySpecDirty(AbilitySpec);
	    	ClientUpdateAbilityStatus(Info.AbilityTag, FAureGameplayTags::Get().Abilities_Status_Eligible);
	    }
	}
}

void UAureAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	//判断Avatar是否基础角色接口
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		//是否有用于可分配点数
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor())>0)
		{
			//调用服务器升级属性函数
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAureAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag);
}

void UAureAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	
	// 初始化Gameplay事件数据Payload
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag; // 设置事件标签为AttributeTag
	Payload.EventMagnitude = 1.f; // 设置事件幅度为1.0
	
	// 向AvatarActor发送Gameplay事件
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	
	// 检查AvatarActor是否实现了UPlayerInterface
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
	    // 调用UPlayerInterface的AddToAttributePoints方法减少属性点
	    IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAureAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
    // for (FGameplayTag Tag : TagContainer)
    // {
	   //  const FString Msg = FString::Printf(TEXT("Effect Applied(GE Tag): %s"), *Tag.ToString());
    // 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Msg);//  调试信息  -1表示不会被覆盖
    // }
	
	
}

