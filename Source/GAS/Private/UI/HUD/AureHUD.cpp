// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/HUD/AureHUD.h"

#include "UI/Widget/AureUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAureHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	// 检查OverlayWidgetController是否尚未初始化
	if (OverlayWidgetController == nullptr)
	{
	    // 如果未初始化，则创建一个新的UOverlayWidgetController实例
	    // 这里的NewObject函数用于根据OverlayWidgetControllerClass动态分配内存并构造对象
	    OverlayWidgetController = NewObject<UOverlayWidgetController>(this,  OverlayWidgetControllerClass);
	    
	    // 设置新创建的OverlayWidgetController的参数
	    // WCParams包含了初始化OverlayWidgetController所需的各种参数
	    OverlayWidgetController->SetWidgetControllerParams(WCParams);

		// 绑定回调函数到依赖项
		// 为了确保OverlayWidgetController能够响应依赖项的状态变化
		OverlayWidgetController->BindCallbacksToDependencies();
		
		// 返回OverlayWidgetController实例
		// 以便于其他部分的代码可以使用这个控制器来管理覆盖层小部件
		//return OverlayWidgetController;
	}
	
	// 返回初始化或已存在的OverlayWidgetController实例
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAureHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	// 检查属性菜单控制器是否尚未被初始化
	if (AttributeMenuWidgetController == nullptr)
	{
	    // 使用指定的类创建一个新的属性菜单控制器实例
	    AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this,
	        AttributeMenuWidgetControllerClass);
	    
	    // 设置控制器的参数
	    AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
	    
	    // 绑定控制器所需的回调函数
	    AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	// 返回初始化或已存在的属性菜单控制器实例
	return AttributeMenuWidgetController;
}

void AAureHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass 未设置，请在HUD上面设置"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass 未设置，请在HUD上面设置"));

	//创建控件Overlay
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAureUserWidget>(Widget);

	//控制器参数结构体
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);//获取控制器层

	OverlayWidget->SetWidgetController(WidgetController);//设置用户控件的控制器层
	OverlayWidgetController->BroadcastInitialValues();//广播初始值
	Widget->AddToViewport();//添加到视口
}
