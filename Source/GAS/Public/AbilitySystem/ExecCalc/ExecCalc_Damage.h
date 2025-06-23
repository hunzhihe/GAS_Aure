// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	// 构造函数，用于初始化UExecCalc_Damage类的实例
	UExecCalc_Damage();
	
	/**
	 * 执行计算伤害的实现函数
	 * 
	 * @param ExecutionParams 游戏效果自定义执行参数，包含计算伤害所需的各种输入数据
	 * @param OutExecutionOutput 执行输出，通过此参数返回计算结果
	 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
