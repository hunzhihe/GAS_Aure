// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AureWidgetController.generated.h"

class UAureAttributeSet;
class UAureAbilitySystemComponent;
class AAurePlayerState;
class AAurePlayerController;
class UAttributeSet;
class UAbilitySystemComponent;

//该委托用来在蓝图中监听技能信息
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAureAbilityInfo&, Info);
// 该委托用来在蓝图中监听属性信息，例如技能点，属性点
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);

// 定义一个结构体FWidgetControllerParams，用于存储小部件控制器的参数
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
    GENERATED_BODY()

    // 默认构造函数
    FWidgetControllerParams() {}
    // 构造函数，初始化成员变量
    FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
    : PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

    // 成员变量，用于存储玩家控制器指针
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<APlayerController> PlayerController = nullptr;

    // 成员变量，用于存储玩家状态指针
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<APlayerState> PlayerState = nullptr;

    // 成员变量，用于存储能力系统组件指针
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

    // 成员变量，用于存储属性集指针
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};

/**
 * 
 */
UCLASS()
class GAS_API UAureWidgetController : public UObject
{
	GENERATED_BODY()

public:
	//蓝图调用设置控制器参数
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	//广播初始化函数
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();

	//绑定数值变动后回调的广播
	virtual void BindCallbacksToDependencies();

	//绑定技能信息监听回调的广播
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;


// 用于UI控制器的属性，仅在蓝图中可读
protected:
	// 存储玩家控制器的指针，允许UI访问玩家控制器的相关信息
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	// 存储玩家状态的指针，允许UI访问玩家特定状态信息
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	// 存储能力系统组件的指针，允许UI访问和显示玩家的能力相关信息
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 存储属性集的指针，允许UI访问玩家的属性信息，如生命值、魔法值等
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAurePlayerController> AurePlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAurePlayerState> AurePlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAureAbilitySystemComponent> AureAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAureAttributeSet> AureAttributeSet;


	AAurePlayerController* GetAurePC();
	AAurePlayerState* GetAurePS() ;
	UAureAbilitySystemComponent* GetAureASC();
	UAureAttributeSet* GetAureAS();
};
