// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AurePlayerState.h"
#include "UI/HUD/AureHUD.h"
#include "UI/WidgetController/AureWidgetController.h"

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
