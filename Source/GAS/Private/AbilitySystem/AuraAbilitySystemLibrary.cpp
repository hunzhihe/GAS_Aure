// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AureAbilityTypes.h"
#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/DamageEvents.h"
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

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(
	const UObject* WorldContextObject)
{
	// 创建一个FWidgetControllerParams类型的对象WCParams，用于存储小部件控制器的参数
	// 获取AuraHUD对象，用于获取属性菜单小部件控制器
	FWidgetControllerParams WCParams;
	AAureHUD* AuraHUD = nullptr;
	
	// 调用MakeWidgetControllerParams函数，根据WorldContextObject生成小部件控制器的参数
	// 如果函数返回true，表示成功生成了控制器参数并且获取了有效的AuraHUD对象
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
	}

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

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const AAureGameModeBase* GameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr) return nullptr;

	//返回关卡的角色的配置
	return  GameMode->AbilityInfo;
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

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	//如果数量过于少，直接返回原数组
	if(Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	//没有引用就是复制，复制一份用于遍历
	TArray<AActor*> ActorsToCheck = Actors;
	//当前已经遍历出最近距离的个数
	int32 NumTargetFound = 0; 

	//循环遍历，直到获得足够数量的目标时停止
	while (NumTargetFound < MaxTargets)
	{
		//如果没有可遍历内容，将跳出循环
		if(ActorsToCheck.Num() == 0) break;
		//记录中心于目标的位置，如果有更小的将被替换，默认是最大
		double ClosestDistance = TNumericLimits<double>::Max();
		//缓存当前最近距离的目标
		AActor* ClosestActor; 
		for(AActor* PotentialTarget : ActorsToCheck)
		{
			//获取目标和中心的距离
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();

			//比对当前计算的位置是否小于缓存的位置
			if(Distance < ClosestDistance)
			{
				//如果小于，将替换对应信息
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		
		//从遍历数组中删除缓存的对象
		ActorsToCheck.Remove(ClosestActor);
		//添加到返回的数组中
		OutClosestTargets.AddUnique(ClosestActor); 
		++ NumTargetFound; //递增数量
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

	//设置死亡冲量
	SetDeathImpulse(EffectContexthandle, DamageEffectParams.DeathImpulse);

	//设置攻击击退
	SetKnockbackForce( EffectContexthandle, DamageEffectParams.KnockbackForce);

	//设置范围伤害相关属性
	SetIsRadialDamage(EffectContexthandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageOrigin(EffectContexthandle, DamageEffectParams.RadialDamageOrigin);
	SetRadialDamageInnerRadius(EffectContexthandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContexthandle, DamageEffectParams.RadialDamageOuterRadius);
		
	// 创建一个游戏效果规范句柄，用于具体化伤害效果的配置，包括效果类、能力等级和上下文信息
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContexthandle);

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

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumRotators)
{
	// 声明一个FRotator类型的动态数组，用于存储计算出的旋转值
	TArray<FRotator> Rotators;
	
	// 计算相对于Forward方向，左侧最边缘的方向向量
	const FVector LeftOfSptead = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	
	// 根据旋转生成器的数量生成相应的旋转值
	if (NumRotators > 1)
	{
	    // 计算相邻两个旋转值之间的角度差
	    const float DeltaSpread = Spread / NumRotators;
	
	    // 为每个旋转生成器计算一个旋转值，并添加到数组中
	    for (int32 i = 0; i < NumRotators; i++)
	    {
	        // 计算当前旋转生成器的方向向量
	        const FVector Direction = LeftOfSptead.RotateAngleAxis(DeltaSpread * (i + 0.5f), Axis);
	        // 将方向向量转换为旋转值，并添加到数组中
	        Rotators.Add(Direction.Rotation());
	    }
	}
	else
	{
	    // 如果只有一个旋转生成器，直接使用Forward方向的旋转值
	    Rotators.Add(Forward.Rotation());
	}
	
	// 返回计算出的旋转值数组
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis,
	float Spread, int32 NumVectors)
{
	// 初始化一个数组，用于存储生成的方向向量
		TArray<FVector> Vectors;
		
	// 计算散射范围的左侧边界向量
		const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
		
	// 如果向量数量大于1，则根据散射角度生成多个方向向量
		if(NumVectors > 1)
		{
	// 计算相邻两个向量之间的角度差
			const float DeltaSpread = Spread / NumVectors;
	 
	// 循环生成每个方向向量
			for(int32 i=0; i<NumVectors; i++)
			{
	// 根据散射角度和当前索引计算并生成方向向量
				const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5f), Axis);
	// 将生成的方向向量添加到数组中
				Vectors.Add(Direction);
			}
		}
	// 如果向量数量等于1，直接使用原始向前方向作为方向向量
		else
		{
			Vectors.Add(Forward);
		}
	 
	// 返回生成的方向向量数组
		return Vectors;
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

float UAuraAbilitySystemLibrary::ApplyRadialDamageWithFalloff(AActor* TargetActor, float BaseDamage,
	float MinimumDamage, const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff,
	AActor* DamageCauser, AController* InstigatedByController, ECollisionChannel DamagePreventionChannel)
{
	// 判断目标角色是否死亡
	bool bIsDead = true;
	if(TargetActor->Implements<UCombatInterface>())
	{
		bIsDead = ICombatInterface::Execute_IsDead(TargetActor);
	}
	if(bIsDead)
	{
		//如果角色已经死亡，直接返回0
		return 0.f; 
	}

	// 获取目标角色所有组件
	TArray<UActorComponent*> Components;
	TargetActor->GetComponents(Components);

	//判断攻击是能能够查看到目标
	bool bIsDamageable = false;

	//存储目标收到碰撞查询到的碰撞结果
	TArray<FHitResult> HitList; 
	for (UActorComponent* Comp : Components)
	{
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Comp);
		if (PrimitiveComp && PrimitiveComp->IsCollisionEnabled())
		{
			FHitResult Hit;
			bIsDamageable = ComponentIsDamageableFrom(
				PrimitiveComp, Origin, DamageCauser, {}, DamagePreventionChannel, Hit
			);
			HitList.Add(Hit);
			if(bIsDamageable) break;
		}
	}

	//应用目标的伤害值
	float AppliedDamage = 0.f;

	if (bIsDamageable)
	{
		// 创建伤害事件
		FRadialDamageEvent DmgEvent;
		DmgEvent.DamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		DmgEvent.Origin = Origin;
		DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageOuterRadius, DamageFalloff);
		DmgEvent.ComponentHits = HitList;
		
		// 应用伤害
		AppliedDamage = TargetActor->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);
	}

	return AppliedDamage;

}

bool UAuraAbilitySystemLibrary::ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin,
	AActor const* IgnoredActor, const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel,
	FHitResult& OutHitResult)
{
	
		// 配置碰撞查询参数，忽略指定的 Actor
		FCollisionQueryParams LineParams(SCENE_QUERY_STAT(ComponentIsVisibleFrom), true, IgnoredActor);
		LineParams.AddIgnoredActors( IgnoreActors );

		// 获取组件所在世界的指针
		UWorld* const World = VictimComp->GetWorld();
		check(World);

		// 使用组件的包围盒中心作为射线终点
		FVector const TraceEnd = VictimComp->Bounds.Origin;
		FVector TraceStart = Origin;
		// 如果起点和终点重合，微调起点以避免提前退出
		if (Origin == TraceEnd)
		{
			// 微调 Z 轴
			TraceStart.Z += 0.01f;
		}

		// 只有当通道合法时才执行射线检测
		if (TraceChannel != ECollisionChannel::ECC_MAX)
		{
			bool const bHadBlockingHit = World->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, TraceChannel, LineParams);
			//::DrawDebugLine(World, TraceStart, TraceEnd, FLinearColor::Red, true);

			// 如果有阻挡物，检查是否为目标组件
			if (bHadBlockingHit)
			{
				if (OutHitResult.Component == VictimComp)
				{
					// 阻挡物是目标组件，返回 true
					return true;
				}
				else
				{
					// 击中其他阻挡物，记录日志并返回 false
					UE_LOG(LogDamage, Log, TEXT("Radial Damage to %s blocked by %s (%s)"), *GetNameSafe(VictimComp),
						*OutHitResult.GetHitObjectHandle().GetName(), *GetNameSafe(OutHitResult.Component.Get()));
					return false;
				}
			}
		}
		else
		{
			// 如果通道无效，输出警告
			UE_LOG(LogDamage, Warning, TEXT("ECollisionChannel::ECC_MAX is not valid! No falloff is added to damage"));
		}

		// 未击中任何物体，构造一个伪造的 HitResult 假设击中组件中心
		FVector const FakeHitLoc = VictimComp->GetComponentLocation();
		FVector const FakeHitNorm = (Origin - FakeHitLoc).GetSafeNormal();		// 法线指向伤害源
		OutHitResult = FHitResult(VictimComp->GetOwner(), VictimComp, FakeHitLoc, FakeHitNorm);
		return true;
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParams(FDamageEffectParams& DamageEffectParams, bool bIsRadial,
	float InnerRadius, float OutRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OutRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;

}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams,
	FVector KnockbackDirection, float Magnitude)
{
	KnockbackDirection.Normalize();
	if(Magnitude == 0.f)
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
	}
	else
	{
		DamageEffectParams.KnockbackForce = KnockbackDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams,
	FVector ImpulseDirection, float Magnitude)
{
	ImpulseDirection.Normalize();
	if(Magnitude == 0.f)
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * DamageEffectParams.DeathImpulseMagnitude;
	}
	else
	{
		DamageEffectParams.DeathImpulse = ImpulseDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetEffectParamsTargetASC(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
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

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 将EffectContextHandle转换为FAureGameplayEffectContext类型
	if (const FAureGameplayEffectContext* AuraEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 如果转换成功，则返回与死亡相关的脉冲（可能用于游戏中的死亡效果）
	    return AuraEffectContext->GetDeathImpulse();
	}
	// 如果转换失败，表明没有相关的死亡脉冲，返回零向量
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 将EffectContextHandle转换为FAureGameplayEffectContext类型
	// 如果转换成功，则调用GetKnockbackForce方法获取击退力
	// 如果转换失败，则返回零向量，表示没有击退力
	if (const FAureGameplayEffectContext* AuraEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    return AuraEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 将EffectContextHandle转换为FAureGameplayEffectContext类型
	if (const FAureGameplayEffectContext* AuraEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 检查AuraEffectContext是否为径向伤害类型
	    return AuraEffectContext->IsRadialDamage();
	}
	// 如果转换失败，则默认返回false
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 将EffectContextHandle转换为FAureGameplayEffectContext类型
	if (const FAureGameplayEffectContext* AuraEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 如果转换成功，则返回径向伤害的内半径
	    return AuraEffectContext->GetRadialDamageInnerRadius();
	}
	// 如果转换失败，则返回0.0
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 尝试将EffectContextHandle转换为FAureGameplayEffectContext类型
	if (const FAureGameplayEffectContext* AuraEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 如果转换成功，则返回径向损害的外半径
	    return AuraEffectContext->GetRadialDamageOuterRadius();
	}
	// 如果转换失败，则返回0.0
	return 0.f;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 将EffectContextHandle转换为FAureGameplayEffectContext类型
	if (const FAureGameplayEffectContext* AuraEffectContext = static_cast<const FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
	    // 如果转换成功，则返回径向伤害的起源位置
	    return AuraEffectContext->GetRadialDamageOrigin();
	}
	// 如果转换失败，则返回零向量，表示没有伤害起源位置
	return FVector::ZeroVector;
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

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InImpulse)
{
	if (FAureGameplayEffectContext* AuraEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InForce)
{
	if (FAureGameplayEffectContext* AuraEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetKnockbackForce(InForce);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsRadialDamage)
{
	if (FAureGameplayEffectContext* AuraEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamage(bInIsRadialDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InInnerRadius)
{
	if (FAureGameplayEffectContext* AuraEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle,
	float InOuterRadius)
{
	if (FAureGameplayEffectContext* AuraEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InOrigin)
{
	if (FAureGameplayEffectContext* AuraEffectContext = static_cast<FAureGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}
