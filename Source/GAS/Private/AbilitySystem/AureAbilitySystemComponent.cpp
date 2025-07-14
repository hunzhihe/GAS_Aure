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
	// 检查输入标签是否有效，如果无效则直接返回
	if(!InputTag.IsValid()) return;
	
	// 遍历所有可激活的能力
	for(auto AbilitySpec : GetActivatableAbilities())
	{
	    // 检查能力规格中的动态源标签是否包含与输入标签完全匹配的标签
	    if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
	    {
	        // 如果找到匹配的标签，则调用能力输入按下处理函数
	        AbilitySpecInputPressed(AbilitySpec);
	    }
	}
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
	    for (FGameplayTag Tag : AbilitySpec.Ability->GetAssetTags())
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
	        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, Info.Level);
	        
	        // 为技能规格添加动态源标签，表明该技能处于可使用状态
	        AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAureGameplayTags::Get().Abilities_Status_Eligible);
	        
	        // 授予玩家该技能
	        GiveAbility(AbilitySpec);
	        
	        // 标记技能规格为脏，以便于后续更新或同步
	        MarkAbilitySpecDirty(AbilitySpec);
	    	ClientUpdateAbilityStatus(Info.AbilityTag, FAureGameplayTags::Get().Abilities_Status_Eligible,AbilitySpec.Level);
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

bool UAureAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
	FString& OutNextLevelDescription)
{
	//如果当前技能处于锁定状态，将无法获取到对应的技能描述
	if (FGameplayAbilitySpec* AureAbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAureGameplayAbility* AureAbility = Cast<UAureGameplayAbility>(AureAbilitySpec->Ability))
		{
			OutDescription = AureAbility->GetDescription(AureAbilitySpec->Level);
			OutNextLevelDescription = AureAbility->GetNextLevelDescription(AureAbilitySpec->Level+1);
			return true;
		}
	}
	//如果技能是锁定状态，将显示锁定状态的技能描述
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid()||AbilityTag.MatchesTagExact(FAureGameplayTags::Get().Abilities_None))
	{
		OutDescription =  FString();
	}
	else
	{
		OutDescription =  UAureGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	
	OutNextLevelDescription = FString();
	return false;
}

void UAureAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Slot)
{
	// 从能力标签获取能力规格
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
	    // 从能力规格中获取当前能力所在的插槽和状态
	    const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
	    const FGameplayTag& Status = GetStatusFromSpec(*AbilitySpec);
	
	    // 获取游戏玩法标签
	    const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
	    // 检查能力状态是否为已装备或已解锁
	    if (Status == GameplayTags.Abilities_Status_Equipped||Status == GameplayTags.Abilities_Status_Unlocked)
	    {
	        // 清除插槽中的所有能力，为新能力腾出空间
	        ClearAbilitiesOfSlot(Slot);
	        // 清除能力规格中的插槽信息
	        ClearSlot(AbilitySpec);
	        // 将新的插槽标签添加到能力规格的动态源标签中
	        AbilitySpec->GetDynamicSpecSourceTags().AddTag(Slot);
	
	        // 如果能力状态为已解锁，则将其更新为已装备
	        if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	        {
	            // 移除已解锁标签
	            AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Unlocked);
	            // 添加已装备标签
	            AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
	        }
	
	        // 向客户端发送装备能力的请求
	        ClientEquipAbility(AbilityTag, Status, Slot, PrevSlot);
	        // 标记能力规格为已更改，以便于后续更新
	        MarkAbilitySpecDirty(*AbilitySpec);
	    }
	}
}

void UAureAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

void UAureAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	// 获取Spec的输入标签
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	
	// 从Spec的动态规范源标签中移除该输入标签
	Spec->GetDynamicSpecSourceTags().RemoveTag(Slot);
	
	// 标记Spec为脏，以便在后续处理中更新或重新计算
	MarkAbilitySpecDirty(*Spec);
}

void UAureAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	// 锁定当前作用域内的所有可激活能力列表，以防止在处理能力时发生并发修改
	FScopedAbilityListLock ActiveScopeLock(*this);
	
	// 遍历所有可激活的能力规格
	for(FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
	    // 检查当前能力规格是否具有指定的插槽
	    if(AbilityHasSlot(&Spec, Slot))
	    {
	        // 清除能力规格中的指定插槽
	        ClearSlot(&Spec);
	    }
	}
}

bool UAureAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	// 遍历Spec的动态规范源标签
	for (FGameplayTag Tag : Spec->GetDynamicSpecSourceTags())
	{
	    // 检查当前标签是否与指定的Slot标签精确匹配
	    if (Tag.MatchesTagExact(Slot))
	    {
	        // 如果匹配到相同的标签，返回true
	        return true;
	    }
	}
	// 如果没有匹配到相同的标签，返回false
	return false;
}

void UAureAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	//获取到技能实例
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		//减少一个可分配技能点
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		//获取状态标签
		FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);
		//根据状态标签处理
		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			//如果技能是可解锁状态，将状态标签从可解锁转换为已解锁
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
			StatusTag = GameplayTags.Abilities_Status_Unlocked;

			//提升技能等级
			AbilitySpec->Level += 1;
		}
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped)||StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		ClientUpdateAbilityStatus(AbilityTag, StatusTag,AbilitySpec->Level);
	}
}

void UAureAbilitySystemComponent::ServerDemotionSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	//获取到技能实例
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		//增加一个可分配的技能点
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), 1);
		}
		//获取状态标签
		FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);

		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec-> Level -= 1;
			if (AbilitySpec->Level<1)
			{
				//技能小于1级，当前技能将无法装配，直接设置为可解锁状态
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Equipped);
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Eligible);
				StatusTag = GameplayTags.Abilities_Status_Eligible;

				//获取技能装配的插槽
				const FGameplayTag& Preslot = GetInputTagFromSpec(*AbilitySpec);
				//清空技能输入标签
				ClearSlot(AbilitySpec);
				ClientEquipAbility(AbilityTag,StatusTag,FGameplayTag(),Preslot);
			}
		}
		ClientUpdateAbilityStatus(AbilityTag, StatusTag,AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAureAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
                                                                           const FGameplayTag& StatusTag,int32 AbilityLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag,AbilityLevel);
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

