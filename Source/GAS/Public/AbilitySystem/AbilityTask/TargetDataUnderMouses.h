// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouses.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 * 
 */
UCLASS()
class GAS_API UTargetDataUnderMouses : public UAbilityTask
{
	GENERATED_BODY()

public:

	/**
	 * 创建一个针对鼠标下目标数据的任务。
	 * 
	 * 此函数用于生成一个任务对象，该对象可以在执行过程中获取鼠标下方的目标数据。
	 * 主要用于游戏能力系统中，当需要根据鼠标位置来确定目标时。
	 * 
	 * @param OwningAbility 拥有此任务的游戏能力对象。该参数标记了哪个能力实例了这个任务。
	 * 
	 * @return 返回一个新创建的UTargetDataUnderMouses对象，用于后续处理鼠标下方的目标数据。
	 * 
	 * @note 
	 *       HidePin="OwningAbility" 和 DefaultToSelf="OwningAbility" 表示拥有此任务的能力对象参数在蓝图中默认隐藏并自动设置为当前能力对象。
	 *       BlueprintInternalUseOnly="true" 表示此函数仅限于在蓝图内部使用。
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouses* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

private:

	// 当能力激活时调用此函数来处理目标数据
	virtual void Activate() override;
	// UTargetDataUnderMouses 类中的 SendMouseCursorData 方法
	// 该方法用于发送鼠标光标下的目标数据到服务端
	void SendMouseCursorData();

	// 当目标数据在客户端复制时调用此函数
	// 该函数处理接收到的复制数据，并根据情况广播有效数据
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
