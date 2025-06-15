// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AureWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
class UAttributeInfo;
struct FAureAttributeInfo;
struct FGameplayTag;
// 声明一个动态多播委托类型FAttributeInfoSignature，用于处理属性信息
// 这个委托接受一个名为Info的参数，类型为const FAureAttributeInfo&
// 该委托类型通常用于事件系统中，以允许多个监听者订阅属性信息事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAureAttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_API UAttributeMenuWidgetController : public UAureWidgetController
{
	GENERATED_BODY()
public:
	// 绑定回调函数到依赖项，用于当依赖项发生变化时自动调用这些回调函数
	virtual void BindCallbacksToDependencies() override;
	
	// 广播初始值，通常用于在系统初始化时将关键属性的初始状态通知给所有相关的组件或系统
	
	virtual void BroadcastInitialValues() override;
	
	// 在GAS（游戏属性系统）类别下，指定属性信息代理，允许在蓝图中绑定事件
	// 用于当属性信息发生变化时通知其他系统或组件
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:

	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
