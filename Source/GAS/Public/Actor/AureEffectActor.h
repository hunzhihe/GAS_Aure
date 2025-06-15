// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AureEffectActor.generated.h"



class USphereComponent;
class UAbilitySystemComponent;
class UGameplayEffect;

// 定义一个枚举类，用于指定效果应用策略
// 此枚举类用于在蓝图中选择效果的应用策略
UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	// 当 overlaps 时应用效果
	ApplyOnOverlap,
	// 当结束 overlap 时应用效果
	ApplyOnEndOverlap,
	// 不应用效果
	DoNotApply
};


// 定义一个枚举类型，用于指定效果的移除策略
UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	// 当不再重叠时移除效果
	RemoveOnEndOverlap,
	// 不移除效果
	DoNotRemove
};


UCLASS()
class GAS_API AAureEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AAureEffectActor();
	virtual void Tick(float DeltaTime) override;
	
protected:
	// 游戏开始或生成对象时回调
	virtual void BeginPlay() override;

	 // 蓝图可调用函数，对指定目标应用游戏效果
	 UFUNCTION(BlueprintCallable)
	 void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
	
	 // 当与某演员发生重叠时调用的蓝图可调用函数
	 UFUNCTION(BlueprintCallable)
	 void OnOverlap(AActor* TargetActor);
	
	 // 当与某演员的重叠结束时调用的蓝图可调用函数
	 UFUNCTION(BlueprintCallable)
	 void OnEndOverlap(AActor* TargetActor);
	
	 // 编辑时可设置，蓝图中只读，控制是否在效果应用时销毁该组件
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 bool bDestroyOnEffectApplication = false;
	
	 // 编辑时可设置，蓝图中只读，控制是否对敌人应用效果
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 bool bApplyEffectsToEnemies = false;
	
	 // 瞬时游戏效果类，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
	 // 瞬时效果的应用策略，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	 // 持续游戏效果类，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	 // 持续效果的应用策略，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	 // 无限游戏效果类，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	 // 无限效果的应用策略，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	 // 无限效果的移除策略，编辑时可设置，蓝图中只读
	 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	 EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;
	
	 // 存储活动的游戏效果句柄和对应的AbilitySystemComponent
	 TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	
	 // 演员等级，编辑时可设置，蓝图中可读写
	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	 float ActorLevel = 1.f;
private:

	

};
