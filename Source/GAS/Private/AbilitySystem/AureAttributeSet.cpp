// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AureAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AureGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "GAS/AureLogChannels.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AurePlayerController.h"

UAureAttributeSet::UAureAttributeSet()
{
	const FAureGameplayTags& GameplayTags = FAureGameplayTags::Get();

	/* Primary Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	/* Secondary Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);	
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	
	/* Resistance Attributes */
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAureAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Primary Attributes
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	// Secondary Attributes

	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// Resistance Attributes

	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	
	// Vital Attributes
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAureAttributeSet, Mana, COND_None, REPNOTIFY_Always);

}

void UAureAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// 如果当前属性是健康值属性
	if (Attribute == GetHealthAttribute())
	{
	    // 将新值限制在0到最大健康值之间
	    NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	// 如果当前属性是魔法值属性
	if (Attribute == GetManaAttribute())
	{
	    // 将新值限制在0到最大魔法值之间
	    NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAureAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAureAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

    FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		// UE_LOG(LogTemp, Warning,
		// 	TEXT("%s 的生命值发生了修改，当前生命值：%f"),
		// 	*Props.TargetAvatarActor->GetName(), GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
    //当传入伤害属性时，并且当前生命值大于0
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute() && GetHealth() > 0.f )
	{
		HandleInComingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		
		UE_LOG(LogAure, Log, TEXT("获取传入经验值：%f"), GetIncomingXP());
		
		HandleIncomingXP(Props);
	}
}

void UAureAttributeSet::HandleInComingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage>0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		// 检查是否死亡
		const bool bFatal = NewHealth <= 0.f;

		//受击动画
		if (!bFatal)
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAureGameplayTags::Get().Effects_HitReact);
			Props.TargetASC->CancelAbilities(&TagContainer);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);

			//判断当前是否应用的负面效果
			if (UAuraAbilitySystemLibrary::IsSuccessfulDeBuff(Props.EffectContextHandle))
			{
				HandleDebuff(Props);
			}

			//设置击退效果
			const FVector& knockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle);
			if (!knockbackForce.IsNearlyZero(1.0f))
			{
				Props.TargetCharacter->LaunchCharacter(knockbackForce, true, true);
			}
		}
		else
		{
			//确认死亡的话调用死亡函数
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
			if (CombatInterface)
			{
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
			}
			
			//发送经验
			SendXPEvent(Props);
		}
	}
	//获取格挡和暴击
	const bool IsBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
	const bool IsCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
		
	ShowFloatingText(Props, LocalIncomingDamage, IsBlockedHit, IsCriticalHit);


}

void UAureAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	//获取负面效果参数
	const FGameplayTag DebuffType = UAuraAbilitySystemLibrary::GetDeBuffDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDeBuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDeBuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDeBuffFrequency(Props.EffectContextHandle);

	//创建GE所使用的名称，并创建一个可实例化的GE
	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DebuffType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	//设置动态创建GE的属性
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = FScalableFloat(DebuffFrequency);//设置间隔
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);//设置持续时间

	Effect->bExecutePeriodicEffectOnApplication = false;//应用后不会立即激发，而是周期到了再触发
	Effect->PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset;//设置每次应用后不会重置触发时间
	
	
	// 设置效果的堆叠类型为按来源聚合，这意味着相同来源的效果将会合并
	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	
	// 设置堆叠限制数量为1，表示最多只能有一个此效果的实例存在
	Effect->StackLimitCount = 1;
	
	// 设置堆叠持续时间刷新策略为在成功应用时刷新，这意味着每次效果成功应用时，其持续时间将重置
	Effect->StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	
	// 设置堆叠周期重置策略为在成功应用时重置，表示每次效果成功应用时，其周期将重新计算
	Effect->StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	
	// 设置堆叠过期策略为清除整个堆叠，这意味着当效果过期时，所有的堆叠层将一起消失
	Effect->StackExpirationPolicy = EGameplayEffectStackingExpirationPolicy::ClearEntireStack;


	// 添加一个UTargetTagsGameplayEffectComponent组件到Effect对象中
	UTargetTagsGameplayEffectComponent& TargetTagsGameplayEffectComponent = Effect->AddComponent<UTargetTagsGameplayEffectComponent>();
	
	// 新建配置的目标标签变化容器
	FInheritedTagContainer InheritableOwnedTagsContainer;
	
	// 向容器中添加一个标签，以标识特定的Debuff类型
	InheritableOwnedTagsContainer.Added.AddTag(DebuffType);
	
	// 设置并应用新的目标标签变化容器
	TargetTagsGameplayEffectComponent.SetAndApplyTargetTagChanges(InheritableOwnedTagsContainer);


	// 获取当前效果的修饰符数量，并将其用作新修饰符的索引
	const int32 Index = Effect->Modifiers.Num();
	
	// 在效果的修饰符列表中添加一个新的修饰符信息
	Effect->Modifiers.Add(FGameplayModifierInfo());
	
	// 获取新添加的修饰符信息的引用，以便进行后续的设置
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];
	
	// 设置修饰符的幅度，这里使用FScalableFloat包装器来处理可缩放的浮点数
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	
	// 设置修饰符的操作类型为加法，这意味着它将与现有的值相加
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	
	// 设置修饰符关联的属性，这里使用的是传入伤害属性
	ModifierInfo.Attribute = UAureAttributeSet::GetIncomingDamageAttribute();


	// 创建一个 gameplay effect context handle
	FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
	// 向 effect context handle 中添加施效者对象
	EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);
	
	// 创建一个 gameplay effect spec 实例
	if (const FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContextHandle, 1.f))
	{
	    // 静态转换获取自定义 effect context
	    FAureGameplayEffectContext* AureContext = static_cast<FAureGameplayEffectContext*>(MutableSpec->GetContext().Get());
	    
	    // 创建并共享 debuff 类型标签
	    const TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DebuffType));
	    
	    // 设置 debuff 的伤害类型
	    AureContext->SetDebuffDamageType(DebuffDamageType);
	    
	    // 应用 gameplay effect spec 到目标身上
	    Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UAureAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	// 获取即将获得的经验值并将其设置为0，为后续的等级提升计算做准备
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);
	
	// 检查目标角色是否实现了玩家接口和战斗接口
	if (Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
	{
	    // 获取当前角色的等级和经验值
	    const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
	    const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);
	
	    // 计算加上即将获得的经验值后的新等级
	    const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter, CurrentXP + LocalIncomingXP);
	    const int32 NumLevelUps = NewLevel - CurrentLevel;
	    // 如果有等级提升，则执行相关操作
	    if (NumLevelUps > 0)
	    {
	        // 将等级提升的次数加到玩家等级上
	        IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
	
	        // 初始化属性点和技能点的奖励
	        int32 AttributePointsReward = 0;
	        int32 SpellPointsReward = 0;
	
	        // 计算等级提升后应获得的属性点和技能点奖励
	        for (int32 i = 0; i < NumLevelUps; ++i)
	        {
	            SpellPointsReward += IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel + i);
	            AttributePointsReward += IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel + i);
	        }
	        
	        // 将计算得到的属性点和技能点奖励加到玩家的属性点和技能点上
	        IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
	        IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);
	    
	        // 设置满血和满蓝的标志，表示等级提升后需要恢复全部生命值和魔法值
	        bTopOffHealth = true;
	        bTopOffMana = true;
	        
	        // 执行等级提升的相关操作
	        IPlayerInterface::Execute_LevelUp(Props.SourceCharacter);
	    }
	    
	    // 将即将获得的经验值加到玩家的经验值上
	    IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAureAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Health, OldHealth);
}

void UAureAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Mana, OldMana);
}

void UAureAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Strength, OldStrength);
}

void UAureAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Intelligence, OldIntelligence);
}

void UAureAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Resilience, OldResilience);
}

void UAureAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Vigor, OldVigor);
}

void UAureAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, Armor, OldArmor);
}

void UAureAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAureAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, BlockChance, OldBlockChance);
}

void UAureAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAureAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAureAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAureAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAureAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAureAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, MaxHealth, OldMaxHealth);
}

void UAureAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, MaxMana, OldMaxMana);
}

void UAureAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, FireResistance, OldFireResistance);
}

void UAureAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, LightningResistance, OldLightningResistance);
}

void UAureAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAureAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAureAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAureAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// 获取效果的上下文句柄
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	// 通过上下文句柄获取原始施法者的AbilitySystemComponent
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	// 如果施法者的AbilitySystemComponent有效，并且其关联的演员信息及头像演员也有效
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
	    // 获取施法者的头像演员和控制器
	    Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
	    Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
	    // 如果控制器为空且头像演员不为空，尝试从头像演员转换为控制器
	    if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
	    {
	        if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
	        {
	            Props.SourceController = Pawn->GetController();
	        }
	    }
	    // 如果控制器不为空，获取其控制的角色
	    if (Props.SourceController)
	    {
	        Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
	    }
	}
	
	// 如果目标的演员信息及头像演员有效
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
	    // 获取目标的头像演员、控制器和角色，并获取目标的AbilitySystemComponent
	    Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	    Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	    Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
	    Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAureAttributeSet::ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsBlockedHit, bool IsCriticalHit)
{
	if (Props.SourceCharacter != Props.TargetCharacter )
	{
		if (AAurePlayerController* PC = Cast<AAurePlayerController>(
			Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit);
		}
		//从目标身上获取PC并显示伤害数字
		if(AAurePlayerController* PC = Cast<AAurePlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit); //调用显示伤害数字
		}
		
	}
}

void UAureAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	// 检查目标角色是否实现了UCombatInterface接口
    if (Props.TargetCharacter->Implements<UCombatInterface>())
    {
    // 通过UCombatInterface接口获取目标角色的玩家等级
    const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
    // 通过UCombatInterface接口获取目标角色的角色类别
    const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
    // 根据目标角色的类别和等级获取经验值奖励
    const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

    // 获取游戏玩法标签实例
    const FAureGameplayTags& GameplayTags = FAureGameplayTags::Get();
    // 初始化游戏玩法事件数据负载
    FGameplayEventData Payload;
    // 设置事件标签为即将获得的经验值
    Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
    // 设置事件数值为经验值奖励
    Payload.EventMagnitude = XPReward;
    // 向源角色发送游戏玩法事件，传递经验值奖励信息
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
    }

}
