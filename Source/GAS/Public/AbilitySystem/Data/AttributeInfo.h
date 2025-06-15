// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

// 定义一个结构体，用于存储属性信息，标记为蓝图类型，表示可以在蓝图中使用
USTRUCT(BlueprintType)
struct FAureAttributeInfo
{
    // 自动生成结构体的实现代码
    GENERATED_BODY()

    // 定义一个 Gameplay 标签，只在编辑默认设置时可见，可以在蓝图中读取
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag AttributeTag = FGameplayTag();

    // 定义属性名称，只在编辑默认设置时可见，可以在蓝图中读取
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AttributeName = FText();

    // 定义属性描述，只在编辑默认设置时可见，可以在蓝图中读取
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AttributeDescription = FText();

    // 定义属性值，只读，可以在蓝图中读取
    UPROPERTY(BlueprintReadOnly)
    float AttributeValue = 0.f;
};


/**
 * 
 */
UCLASS()
class GAS_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	// 根据属性标签查找属性信息
	// AttributeTag: 要查找的属性标签
	// bLogNotFound: 如果未找到属性信息是否记录日志，默认为false
	// 返回: 如果找到则返回属性信息，否则返回FAuraAttributeInfo的默认值
	FAureAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;
	
	// 存储属性信息的数组
	// 仅在编辑默认设置时可编辑，蓝图中可读取
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAureAttributeInfo> AttributeInformation;
};
