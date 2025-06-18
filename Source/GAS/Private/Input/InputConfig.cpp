// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputConfig.h"

#include "GameplayTagContainer.h"


const UInputAction* UInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// 遍历能力输入动作数组，寻找与给定输入标签匹配的能力输入动作
	for (const FAuraInputAction& Action: AbilityInputActions)
	{
	    // 检查动作是否有效且输入标签是否匹配
	    if (Action.InputAction && Action.InputTag == InputTag)
	    {
	        // 如果找到匹配的输入动作，返回该输入动作
	        return Action.InputAction;
	    }
	}
	
	// 如果未找到匹配的能力输入动作且需要记录未找到的情况
	if (bLogNotFound)
	{
	    // 输出错误日志，指示无法找到与输入标签对应的能力输入动作
	    UE_LOG(LogTemp, Error, TEXT("无法从InputConfig[%s]中找到InputTag[%s]对应的InputAction"),  *GetNameSafe(this),*InputTag.ToString());
	}
	
	// 如果没有找到匹配的能力输入动作，返回空指针
	return nullptr;
}
