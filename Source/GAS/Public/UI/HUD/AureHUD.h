// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AureHUD.generated.h"

class UAttributeMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAureUserWidget;
/**
 * 
 */
UCLASS()
class GAS_API AAureHUD : public AHUD
{
	GENERATED_BODY()

public:
	/**
	 * 获取覆盖小部件控制器
	 * 
	 * @param WCParams 小部件控制器参数，包含初始化控制器所需的信息
	 * @return 返回一个UOverlayWidgetController实例，用于管理覆盖小部件的显示和行为
	 */
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	
	/**
	 * 获取属性菜单小部件控制器
	 * 
	 * @param WCParams 小部件控制器参数，包含初始化控制器所需的信息
	 * @return 返回一个UAttributeMenuWidgetController实例，用于管理属性菜单小部件的显示和行为
	 */
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);


private:
	// 在UI中覆盖显示特定的用户界面。
	UPROPERTY()
	TObjectPtr<UAureUserWidget> OverlayWidget;
	
	// OverlayWidget对象的类，允许在编辑器中指定或更改UI覆盖的类型。
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAureUserWidget> OverlayWidgetClass;
	
	 // 控制OverlayWidget的行为和逻辑。
	 UPROPERTY()
	 TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	 // OverlayWidgetController对象的类，允许在编辑器中指定或更改控制UI覆盖逻辑的类型。
	 UPROPERTY(EditAnywhere)
	 TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// 属性菜单小部件控制器的实例
	// 用于管理属性菜单的显示和交互逻辑
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	
	// 属性菜单小部件控制器的类
	// 允许在编辑器中指定自定义的控制器类
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	

	
};
