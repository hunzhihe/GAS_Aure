// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAureAbilityInfo
{
	GENERATED_BODY()

	//技能标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	//技能输入映射标签
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	//技能冷却标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	//技能图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;

	//背景材质
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Background = nullptr;
	
};

UCLASS()
class GAS_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	// 技能信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AbilityInformation")
	TArray<FAureAbilityInfo> AbilityInformation;

	//通过技能标签获取到技能相关数据
	FAureAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag,
		bool bLogNotFound = false) const;
	
};
