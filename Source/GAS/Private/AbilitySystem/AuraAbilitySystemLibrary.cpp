// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AureAbilityTypes.h"
#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/OverlapResult.h"
#include "Game/AureGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AurePlayerState.h"
#include "UI/HUD/AureHUD.h"
#include "UI/WidgetController/AureWidgetController.h"

class AAureGameModeBase;

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,
                                                           FWidgetControllerParams& OutWCParams, AAureHUD*& OutAuraHUD)
{
	// 获取玩家控制器，以便后续获取HUD和玩家状态
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
	    // 尝试将获取到的HUD转换为我们自定义的AAureHUD类型
	    OutAuraHUD = Cast<AAureHUD>(PC->GetHUD());
	    if (OutAuraHUD)
	    {
	        // 获取玩家状态和相关的组件，为后续的操作准备必要的数据
	        AAurePlayerState* PS = PC->GetPlayerState<AAurePlayerState>();
	        UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	        UAttributeSet* AS = PS->GetAttributeSet();
	
	        // 填充输出参数，以便调用者可以使用这些数据
	        OutWCParams.AttributeSet = AS;
	        OutWCParams.AbilitySystemComponent = ASC;
	        OutWCParams.PlayerState = PS;
	        OutWCParams.PlayerController = PC;
	        // 成功获取并填充所有所需数据，返回true
	        return true;
	    }
	}
	// 如果无法获取到玩家控制器或HUD，则视为失败，返回false
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{


	// 创建一个FWidgetControllerParams类型的对象WCParams，用于存储小部件控制器的参数
	// 初始化AuraHUD指针为nullptr，用于后续获取AAureHUD实例
	FWidgetControllerParams WCParams;
	AAureHUD* AuraHUD = nullptr;
	
	// 调用MakeWidgetControllerParams函数，根据WorldContextObject生成小部件控制器的参数
	// 如果函数返回true，表示参数生成成功，并且AuraHUD指针已经被赋值
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
	    // 使用生成的参数从AuraHUD获取Overlay小部件控制器
	    // 这里利用WCParams作为参数，因为获取小部件控制器可能依赖于这些参数
	    return AuraHUD->GetOverlayWidgetController(WCParams);
	}
	
	// 如果参数生成失败，返回nullptr，表示没有成功创建小部件控制器
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	// 创建一个FWidgetControllerParams类型的对象WCParams，用于存储小部件控制器的参数
	// 初始化AuraHUD指针为nullptr，用于后续获取AAureHUD对象
	FWidgetControllerParams WCParams;
	AAureHUD* AuraHUD = nullptr;
	
	// 调用MakeWidgetControllerParams函数，根据WorldContextObject生成小部件控制器的参数
	// 如果函数返回true，表示成功生成了控制器参数并且获取了有效的AuraHUD对象
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
	    // 调用AuraHUD的GetAttributeMenuWidgetController方法，传入控制器参数WCParams
	    // 返回属性菜单小部件控制器对象
	    return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	
	// 如果MakeWidgetControllerParams函数调用失败，返回nullptr，表示没有成功获取到小部件控制器
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	//获取到当前关卡的GameMode实例
	const AAureGameModeBase* GameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr) return;

	//从实例获取到关卡角色的配置
	UCharacterClassInfo* ClassInfo = GameMode->CharacterClassInfo;

	//获取到默认的基础角色数据
	const FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

	//应用基础属性
	FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle PrimarySpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimarySpecHandle.Data.Get());

	//设置次级属性
	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	//填充血量和蓝量
	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
	
	
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const AAureGameModeBase* GameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr) return nullptr;

	//返回关卡的角色的配置
	return  GameMode->CharacterClassInfo;

}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
	ECharacterClass CharacterClass)
{
	// 获取角色类信息
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	// 如果角色类信息为空，则不执行后续操作
	if (CharacterClassInfo == nullptr) return;


	//从战斗接口获取到角色的等级
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor());
	int32 CharacterLevel = 1;
	if(CombatInterface)
	{
		CharacterLevel = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());
	}
	
	//遍历角色拥有的技能数组
	for(const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		//创建技能实例
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CharacterLevel); 

		//只应用不激活
		ASC->GiveAbility(AbilitySpec); 
	}

	// 获取指定角色类的默认信息
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	
	// 遍历该角色类在初始化时应具备的技能
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
	    // 检查能力系统组件的化身演员是否实现了战斗接口
	    if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
	    {
	        // 创建技能规格，包括技能类和玩家等级
	        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CharacterLevel);
	        
	        // 给能力系统组件赋予该技能,只应用不激活
	        ASC->GiveAbility(AbilitySpec);
	    }
	}
	
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAureGameplayEffectContext* AureGameplayEffectContext =
		static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AureGameplayEffectContext->IsBlockHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAureGameplayEffectContext* AureGameplayEffectContext =
		static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AureGameplayEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	FAureGameplayEffectContext* AureGameplayEffectContext =
		static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get());
	AureGameplayEffectContext->SetBlockHit(bInIsBlockedHit);
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	FAureGameplayEffectContext* AureGameplayEffectContext =
		static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get());
	AureGameplayEffectContext->SetCriticalHit(bInIsCriticalHit);
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams; //创建一个碰撞查询的配置
	SphereParams.AddIgnoredActors(ActorsToIgnore); //添加忽略的Actor
	
	TArray<FOverlapResult> Overlaps; //创建存储检索到的与碰撞体产生碰撞的Actor
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) //获取当前所处的场景，如果获取失败，将打印并返回Null
	{
		//获取到所有与此球体碰撞的动态物体
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), SphereParams);
		for(FOverlapResult& Overlap : Overlaps) //遍历所有获取到的动态Actor
		{
			//判断当前Actor是否包含战斗接口   Overlap.GetActor() 从碰撞检测结果中获取到碰撞的Actor
			const bool ImplementsCombatInterface =  Overlap.GetActor()->Implements<UCombatInterface>();
			//判断当前Actor是否存活，如果不包含战斗接口，将不会判断存活（放置的火堆也属于动态Actor，这样保证不会报错）
			if(ImplementsCombatInterface && !ICombatInterface::Execute_IsDead(Overlap.GetActor())) 
			{
				OutOverlappingActors.AddUnique(Overlap.GetActor()); //将Actor添加到返回数组，AddUnique 只有在此Actor未被添加时，才可以添加到数组
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	// 判断两个指定的Actor是否都不是朋友
	// 此函数通过检查两个Actor是否都标记为"Player"或"Enemy"来确定它们是否为朋友
	// 如果两个Actor都是玩家或都是敌人，则它们被视为朋友，函数返回false
	// 如果它们不是朋友关系，则函数返回true
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	// 获取游戏玩法标签的引用，用于后续可能的标签操作
	const FAureGameplayTags& GameplayTags = FAureGameplayTags::Get();
	
	// 获取伤害效果来源的演员对象，这是施加伤害效果的实体
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
		
	// 创建一个游戏效果上下文句柄，用于描述伤害效果的来源和环境信息
	FGameplayEffectContextHandle EffectContexthandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	
	// 将来源演员对象添加到效果上下文中，以便在应用伤害效果时考虑来源的信息
	EffectContexthandle.AddSourceObject(SourceAvatarActor);
		
	// 创建一个游戏效果规范句柄，用于具体化伤害效果的配置，包括效果类、能力等级和上下文信息
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContexthandle);

	//通过标签设置GE使用的配置
	for(auto& Pair : DamageEffectParams.DamageTypes)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Pair.Value);
	}
	// 通过蓝图库函数，根据伤害类型和基础伤害值设置效果规范的属性
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DebuffDamageType, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);
	
	// 应用配置好的伤害效果到目标的能力系统组件上
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	// 返回创建的效果上下文句柄，可能用于后续的跟踪或处理
	return EffectContexthandle;
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 CharacterLevel)
{
	// 获取角色类信息对象
    UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
    // 如果角色类信息对象为空，则返回0
    if (CharacterClassInfo == nullptr) return 0;

    // 获取指定角色类的默认信息
    const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
    // 根据角色等级获取经验值奖励
    const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

    // 将浮点数经验值奖励转换为整型并返回
    return static_cast<int32>(XPReward);

}

bool UAuraAbilitySystemLibrary::IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 将EffectContextHandle转换为FAureGameplayEffectContext类型
	if (const FAureGameplayEffectContext* AureEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 检查转换后的AuraEffectContext是否表示一个成功的DeBuff效果
	    return AureEffectContext->IsSuccessfulDeBuff();
	}
	// 如果转换失败，则默认返回false
	return false;
}

float UAuraAbilitySystemLibrary::GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAureGameplayEffectContext* AureEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 返回AuraEffectContext中设置的DeBuff伤害值
	    return AureEffectContext->GetDeBuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAureGameplayEffectContext* AureEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 返回AuraEffectContext中设置的DeBuff持续时间
	    return AureEffectContext->GetDeBuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAureGameplayEffectContext* AureEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 返回AuraEffectContext中设置的DeBuff频率
	    return AureEffectContext->GetDeBuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDeBuffDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAureGameplayEffectContext* AureEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		//如果当前存在设置了伤害类型
		if (AureEffectContext->GetDebuffDamageType().IsValid())
		{
			return *AureEffectContext->GetDebuffDamageType();
		}
	}
	return FGameplayTag();
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDeBuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsSuccessfulDeBuff)
{
	FAureGameplayEffectContext* AureEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get());
	AureEffectContext->SetSuccessfulDeBuff(bInIsSuccessfulDeBuff);
	
}

void UAuraAbilitySystemLibrary::SetDeBuff(FGameplayEffectContextHandle& EffectContextHandle, FGameplayTag& InDamageType,
	float InDamage, float InDuration, float InFrequency)
{
	FAureGameplayEffectContext* AureEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get());

	const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
	AureEffectContext->SetDeBuffDamage(InDamage);
	AureEffectContext->SetDeBuffDuration(InDuration);
	AureEffectContext->SetDeBuffFrequency(InFrequency);
	AureEffectContext->SetDebuffDamageType(DamageType);
	
}
