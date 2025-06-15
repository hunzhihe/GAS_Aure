// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AureEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "Components/SphereComponent.h"

// Sets default values
AAureEffectActor::AAureEffectActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	
}

// Called when the game starts or when spawned
void AAureEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAureEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// 如果目标演员标记为"Enemy"且不应对敌人应用效果，则不执行后续操作
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies)
	{
	    return;
	}

	// 获取目标Actor的AbilitySystemComponent组件
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	// 如果目标组件为空，则直接返回
	if (TargetASC == nullptr) return;
	
	// 确保GameplayEffectClass有效性
	check(GameplayEffectClass);
	// 创建游戏效果上下文句柄
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	// 添加效果来源对象
	EffectContextHandle.AddSourceObject(this);
	// 创建游戏效果规范句柄
	const FGameplayEffectSpecHandle EffectSpecHandle =
	    TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	// 应用游戏效果并获取激活效果句柄
	const FActiveGameplayEffectHandle ActiveEffectHandle =
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
	// 判断效果是否为无限持续类型
	const bool bIsInfinite =  EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	// 如果是无限持续效果且移除策略为在结束重叠时移除，则将激活效果句柄与目标组件添加到激活效果列表
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
	    ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
	
	// 如果不是无限持续效果，则销毁此对象
	if (!bIsInfinite)
	{
	    Destroy();
	}
	
}

void AAureEffectActor::OnOverlap(AActor* TargetActor)
{
	// 如果目标演员具有"Enemy"标签且不应对敌人应用效果，则返回
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	// 如果即时效果应用策略为在重叠时应用，则对目标演员应用即时游戏玩法效果
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
	    ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	
	// 如果持续效果应用策略为在重叠时应用，则对目标演员应用持续游戏玩法效果
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
	    ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	
	// 如果无限效果应用策略为在重叠时应用，则对目标演员应用无限游戏玩法效果
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
	    ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAureEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// 如果目标演员带有"Enemy"标签且不应对敌人应用效果，则直接返回
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	// 如果即时效果的应用策略是在结束重叠时应用，则对目标应用即时游戏玩法效果
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
	    ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	
	// 如果持续效果的应用策略是在结束重叠时应用，则对目标应用持续游戏玩法效果
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
	    ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	
	// 如果无限效果的应用策略是在结束重叠时应用，则对目标应用无限游戏玩法效果
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
	    ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	
	// 如果无限效果的移除策略是在结束重叠时移除，则获取目标的技能系统组件并移除活跃的游戏玩法效果
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
	    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	    if (!IsValid(TargetASC)) return;
	
		// 遍历ActiveEffectHandles，移除与TargetASC关联的所有效果句柄
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
		    // 如果当前效果句柄关联的AbilitySystemComponent是目标组件
		    if (TargetASC == HandlePair.Value)
		    {
		        // 从目标组件中移除对应的效果实例
		        TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
		        // 将需要移除的效果句柄添加到列表中
		        HandlesToRemove.Add(HandlePair.Key);
		    }
		}
		
		// 从ActiveEffectHandles中移除所有已处理的效果句柄
		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
		    ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

// Called every frame
void AAureEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



