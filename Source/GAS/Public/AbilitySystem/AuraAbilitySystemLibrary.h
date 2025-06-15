// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

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

};
