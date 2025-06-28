// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	// 从给定的世界上下文对象中获取属性菜单小部件控制器
	// 此函数用于在蓝图中以纯函数的形式获取UAttributeMenuWidgetController实例
	// 参数: WorldContextObject - 用于获取控制器的世界上下文对象
	// 返回值: 返回UAttributeMenuWidgetController的实例，用于控制属性菜单小部件的行为和属性
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	

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

	/*--------游戏效果上下文获取--------*/
	//是否格挡
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	//是否暴击
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	//设置是否格挡 UPARAM(ref)实现蓝图节点左侧参数
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	//设置是否暴击
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	/*--------Gameplay Mechanics--------*/
	//获取到攻击位置半径内的所有动态Actor
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject,
		TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|DamageEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	/*---------GameplayEffectDebuff---------*/
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

	//设置GE是否应用负面效果
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetIsSuccessfulDeBuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDeBuff);

	//设置GE负面效果相关数值 负面效果伤害类型 负面效果伤害 负面效果持续时间 负面效果触发间隔时间
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayEffects")
	static void SetDeBuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, FGameplayTag& InDamageType, float InDamage, float InDuration, float InFrequency);

	
};
