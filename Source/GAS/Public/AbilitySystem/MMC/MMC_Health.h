// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_Health.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UMMC_Health : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_Health();

	/**
	 * 虚拟函数，用于计算基础属性值的大小
	 * 
	 * @param Spec 游戏效果规范，用于计算基础属性值
	 * @return 返回计算出的基础属性值大小
	 */
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	// 定义一个游戏效果属性捕获定义对象，用于描述活力属性的捕获规则
	FGameplayEffectAttributeCaptureDefinition VigorDef;
	
};
