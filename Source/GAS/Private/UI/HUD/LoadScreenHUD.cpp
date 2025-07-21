// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/MVVM/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	//实例化MVVM
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	//测试代码
	LoadScreenViewModel->SetWidgetName("WidgetName"); 

	//创建用户控件并添加到视口
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	
	// 获取当前游戏状态的拥有者玩家控制器
	APlayerController* PC = GetOwningPlayerController();
	
	// 创建一个仅限UI输入模式的对象
	FInputModeUIOnly InputMode;
	
	// 设置该输入模式聚焦到指定的UI小部件上
	InputMode.SetWidgetToFocus(LoadScreenWidget->TakeWidget());
	
	// 设置鼠标锁定模式为不锁定
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	// 将创建的输入模式应用到玩家控制器
	PC->SetInputMode(InputMode);
	
	// 设置鼠标光标为可见
	PC->SetShowMouseCursor(true);

	//创建完成用户控件后，调用用户控件函数
	LoadScreenWidget->BlueprintInitializeWidget();

}
