// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputConfig.h"
#include "AureEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	/**
	 * 绑定能力操作到对应的用户定义函数
	 * 
	 * 此函数模板用于将输入配置中定义的能力操作绑定到用户提供的函数上
	 * 它允许用户指定当某个输入被按下、释放或持续按下时，应该调用哪个函数
	 * 
	 * @param InputConfig 输入配置的常量指针，包含能力操作的映射信息
	 * @param Object 用户类的实例指针，用于调用绑定的成员函数
	 * @param PressedFunc 当输入被按下时调用的函数或lambda表达式
	 * @param ReleasedFunc 当输入被释放时调用的函数或lambda表达式
	 * @param HeldFunc 当输入持续按下时调用的函数或lambda表达式
	 * 
	 * @tparam UserClass 用户类的类型，通常是一个游戏中的角色或玩家控制器
	 * @tparam PressedFuncType 按下操作的函数类型
	 * @tparam ReleasedFuncType 释放操作的函数类型
	 * @tparam HeldFuncType 持续按下操作的函数类型
	 */
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UInputConfig* InputConfig, UserClass* Object,
		PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};


/**
 * 绑定能力操作到相应的输入事件
 * 
 * 该函数通过读取输入配置文件，将特定的能力操作绑定到输入事件处理函数上
 * 它支持三种类型的输入事件：按下、释放和持续按住，并为每种事件类型分别绑定处理函数
 * 
 * @param InputConfig 输入配置，包含能力操作和对应的输入标签
 * @param Object 用户类的实例，用于调用绑定的函数
 * @param PressedFunc 当输入事件为按下时调用的函数
 * @param ReleasedFunc 当输入事件为释放时调用的函数
 * @param HeldFunc 当输入事件为持续按住时调用的函数
 */
template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAureEnhancedInputComponent::BindAbilityActions(
	const UInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	// 检查输入配置是否有效
	check(InputConfig);

	// 遍历输入配置中的每一个能力操作
	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		// 确保当前操作的输入动作和输入标签都有效
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			// 如果提供了按下事件的处理函数，则绑定该函数到输入动作的开始事件
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			// 如果提供了释放事件的处理函数，则绑定该函数到输入动作的完成事件
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			
			// 如果提供了持续按住事件的处理函数，则绑定该函数到输入动作的触发事件
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
