// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InputConfig.generated.h"


// 定义一个结构体FAuraInputAction，用于绑定输入动作和对应的 gameplay 标签
USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	// 定义一个只在编辑默认设置时可见的属性，用于绑定一个输入动作
	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	// 定义一个只在编辑默认设置时可见的属性，用于指定与输入动作关联的 gameplay 标签
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};
/**
 * 
 */
UCLASS()
class GAS_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	// 根据输入标签查找对应的能力输入操作
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;
	
	// 存储所有能力输入操作的数组
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
};
