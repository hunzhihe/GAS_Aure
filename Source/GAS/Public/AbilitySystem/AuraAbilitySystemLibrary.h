// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AuraAbilitySystemLibrary.generated.h"

class ULootTiers;
class ULocalScreenSaveGame;
class UCharacterClassInfo;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class AAureHUD;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class GAS_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/*--------Widegt控制器--------*/
	/**
	 * 创建并返回Widget控制器参数的函数
	 * 此函数用于初始化Widget控制器所需的参数，并提供一个 Aura HUD 实例
	 * 主要用于UI相关的控制和显示
	 * 
	 * @param WorldContextObject 世界上下文对象，通常用于获取世界设置或游戏实例信息 默认为调用者自身
	 * @param OutWCParams 输出参数，包含Widget控制器所需的各种参数 由函数填充
	 * @param OutAuraHUD 输出参数，指向Aura HUD实例的指针 由函数填充
	 * 
	 * @return bool 表示是否成功创建并填充了Widget控制器参数
	 * 
	 * @note 此函数标记为BlueprintPure，意味着它没有副作用，只根据输入参数产生输出
	 *       它被归类在"AuraAbilitySystemLibrary|WidgetController"类别下，以便在蓝图中更容易找到和使用
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAureHUD*& OutAuraHUD);
	// 从给定的世界上下文对象中获取覆盖小部件控制器
	// 此函数用于在蓝图中以纯函数的形式获取UOverlayWidgetController实例
	// 参数: WorldContextObject - 用于获取控制器的世界上下文对象
	// 返回值: 返回UOverlayWidgetController的实例，用于控制覆盖小部件的行为和属性
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	// 从给定的世界上下文对象中获取属性菜单小部件控制器
	// 此函数用于在蓝图中以纯函数的形式获取UAttributeMenuWidgetController实例
	// 参数: WorldContextObject - 用于获取控制器的世界上下文对象
	// 返回值: 返回UAttributeMenuWidgetController的实例，用于控制属性菜单小部件的行为和属性
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	//获取属性面板的控制器
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	

	/*--------角色类默认属性--------*/
	//初始化角色的属性
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	//获取角色配置数据
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	//初始化敌人角色技能
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,ECharacterClass CharacterClass);

	//获取玩家角色技能配置数据
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	/*--------游戏效果上下文获取和设置--------*/
	//是否格挡
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	//是否暴击
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	//获取当前GE是否成功应用负面效果
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static bool IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果伤害
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static float GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果持续时间
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static float GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果触发间隔
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static float GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE负面效果伤害类型
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static FGameplayTag GetDeBuffDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	// 从指定的游戏效果上下文中获取死亡冲击力的方向和强度
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	
	// 从指定的游戏效果上下文中获取击退力的方向和强度
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle);
	
	// 检查指定的游戏效果上下文是否包含径向损伤信息
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	
	// 从指定的游戏效果上下文中获取径向损伤的内半径
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	
	// 从指定的游戏效果上下文中获取径向损伤的外半径
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	
	// 从指定的游戏效果上下文中获取径向损伤的起源点
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	//----------------

	//设置是否格挡 UPARAM(ref)实现蓝图节点左侧参数
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	//设置是否暴击
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	
	//设置GE是否应用负面效果
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDeBuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDeBuff);

	//设置GE负面效果相关数值 负面效果伤害类型 负面效果伤害 负面效果持续时间 负面效果触发间隔时间
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetDeBuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, FGameplayTag& InDamageType, float InDamage, float InDuration, float InFrequency);

	// 设置死亡冲量
	// EffectContextHandle: 效果上下文句柄，用于传递和修改游戏效果相关数据
	// InImpulse: 死亡时的冲量，用于控制角色的死亡动画或移动
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse);
	
	// 设置击退力
	// EffectContextHandle: 效果上下文句柄，用于传递和修改游戏效果相关数据
	// InForce: 击退力的大小，用于控制角色的击退动画或移动
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackForce(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce);
	
	// 设置是否为径向伤害
	// EffectContextHandle: 效果上下文句柄，用于传递和修改游戏效果相关数据
	// bInIsRadialDamage: 是否为径向伤害的标志，用于控制伤害的计算方式
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);
	
	// 设置径向伤害内半径
	// EffectContextHandle: 效果上下文句柄，用于传递和修改游戏效果相关数据
	// InInnerRadius: 径向伤害的内半径，用于控制伤害的范围
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius);
	
	// 设置径向伤害外半径
	// EffectContextHandle: 效果上下文句柄，用于传递和修改游戏效果相关数据
	// InOuterRadius: 径向伤害的外半径，用于控制伤害的范围
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius);
	
	// 设置径向伤害起源点
	// EffectContextHandle: 效果上下文句柄，用于传递和修改游戏效果相关数据
	// InOrigin: 径向伤害的起源点，用于控制伤害的计算位置
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin);

	/*--------Gameplay Mechanics--------*/
	//获取到攻击位置半径内的所有动态Actor
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject,
		TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	/**
	 * 获取距离目标位置最近的几个目标
	 * @param MaxTargets 获取最大目标的数量
	 * @param Actors 需要计算的目标数组
	 * @param OutClosestTargets 返回获取到的最近的目标
	 * @param Origin 计算的位置
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);
	

	
	//是否友军
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);
	
	//通过技能生成的负面配置项应用技能负面效果
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);
	

	// 生成均匀分布的多端角度数组
	// 
	// 参数:
	// - Forward: 前进方向向量
	// - Axis: 旋转轴向量
	// - Spread: 分散角度，决定旋转器之间的角度差
	// - NumRotators: 生成的旋转器数量
	// 
	// 返回值:
	// 返回一个包含多个均匀分布的旋转器（FRotator）的数组
	// 
	// 说明:
	// 该函数用于在指定的前进方向和旋转轴上生成多个均匀分布的旋转器，可以用于如技能发射等游戏机制中
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators);
	
	// 生成均匀分布的多段朝向数组
	// 
	// 参数:
	// - Forward: 前进方向向量
	// - Axis: 旋转轴向量
	// - Spread: 分散角度，决定向量之间的角度差
	// - NumVectors: 生成的向量数量
	// 
	// 返回值:
	// 返回一个包含多个均匀旋转的向量（FVector）的数组
	// 
	// 说明:
	// 该函数用于在指定的前进方向和旋转轴上生成多个均匀旋转的向量，适用于需要在多个方向上应用力或移动的场景
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors);

	//通过敌人类型和等级获取XP奖励
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);


	/**
 * 应用径向衰减伤害的函数
 * 
	* @param TargetActor - 需要计算攻击的目标
	 * @param BaseDamage - 在伤害内半径（DamageInnerRadius）内应用的最大伤害值。
	 * @param MinimumDamage - 在伤害外半径（DamageOuterRadius）处应用的最小伤害值。如果为0将不受伤害
	 * @param Origin - 爆炸的原点（中心位置），即伤害的起点。
	 * @param DamageInnerRadius - 全伤害半径：在该范围内的所有对象会受到最大伤害（BaseDamage）。
	 * @param DamageOuterRadius - 最小伤害半径：在该范围之外的对象只会受到**MinimumDamage**。
	 * @param DamageFalloff - 控制伤害递减的速率。值越高，伤害递减得越快。
	 * @param DamageCauser - 伤害的直接来源，如爆炸的手雷或火箭弹。
	 * @param InstigatedByController - 造成伤害的控制器，通常是执行该行为的玩家控制器。
	 * @param DamagePreventionChannel - 阻挡伤害的通道。如果某个对象阻挡了该通道上的检测，则不会对目标应用伤害（如墙壁阻挡了视线）
 * 
 * @return 返回应用的伤害值
 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="RPGAbilitySystemLibrary|GameplayMechanics", meta=(WorldContext="WorldContextObject", AutoCreateRefTerm="IgnoreActors"))
	static float ApplyRadialDamageWithFalloff(AActor* TargetActor, float BaseDamage, float MinimumDamage, const FVector& Origin,
		float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, AActor* DamageCauser = NULL,
		AController* InstigatedByController = NULL, ECollisionChannel DamagePreventionChannel = ECC_Visibility);

	/** @RETURN 如果从 Origin 发出的武器射线击中了 VictimComp 组件，则返回 True。 OutHitResult 将包含击中的具体信息。 */
	static bool ComponentIsDamageableFrom(UPrimitiveComponent* VictimComp, FVector const& Origin, AActor const* IgnoredActor,
		const TArray<AActor*>& IgnoreActors, ECollisionChannel TraceChannel, FHitResult& OutHitResult);


	/*
	* Damage Effect Params
	*/

	/**
		 * 修改伤害配置项，将其设置为具有范围伤害的配置项
		 * @param DamageEffectParams 需要修改的配置项
		 * @param bIsRadial 设置是否为范围伤害
		 * @param InnerRadius 内半径
		 * @param OutRadius 外半径
		 * @param Origin 伤害中心
		 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamageEffectParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams, bool bIsRadial, float InnerRadius, float OutRadius, FVector Origin);

	/**
	 * 修改伤害的冲击力的方向
	 * @param DamageEffectParams 需要修改的伤害配置项
	 * @param KnockbackDirection 攻击时触发击退的方向
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetKnockbackDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude = 0.f);
	
	/**
	 * 修改伤害的冲击力的方向
	 * @param DamageEffectParams 需要修改的伤害配置项
	 * @param ImpulseDirection 死亡时触发击退的方向
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetDeathImpulseDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude = 0.f);

	/**
	 * 设置伤害配置应用目标ASC
	 * @param DamageEffectParams 需要修改的伤害配置 
	 * @param InASC 应用目标ASC
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetEffectParamsTargetASC(UPARAM(ref) FDamageEffectParams& DamageEffectParams, UAbilitySystemComponent* InASC);

	/**
	 * 从存档初始化角色的属性
	 *
	 * @param WorldContextObject 一个世界场景的对象，用于获取当前所在的世界
	 * @param ASC 角色的技能系统组件
	 * @param SaveGame 角色使用的存档指针
	 *
	 * @return void 
	 *
	 * @note 这个函数主要用于从存档里读取角色信息，并初始化
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf = "WorldContextObject"))
	static void InitializeDefaultAttributesFromSaveData(const UObject* WorldContextObject, UAbilitySystemComponent* ASC,
		ULocalScreenSaveGame* SaveGame);


	/*
	 * 获取生成的战利品数据资产，此数据被配置到GM上
	 * @param WorldContextObject 一个世界场景的对象，用于获取当前所在的世界
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf = "WorldContextObject"))
	static ULootTiers* GetLootTiers(const UObject* WorldContextObject);
	
};