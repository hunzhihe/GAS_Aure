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
	 * 根据执行参数、效果规范和评估参数来确定减益效果
	 * 
	 * @param ExecutionParams 执行参数，包含了执行效果时的上下文信息
	 * @param Spec 游戏玩法效果的规范，定义了效果的特性
	 * @param EvaluationParameters 评估参数，用于影响属性聚合和评估的方式
	 * @param InTagsToDefs 标签到属性捕获定义的映射，用于确定哪些属性需要捕获和如何捕获
	 */
	void DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                     const FGameplayEffectSpec& Spec,
	                     FAggregatorEvaluateParameters EvaluationParameters,
	                     const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToCaptrueDefs) const;
	
	/**
	 * 执行计算伤害的实现函数
	 * 
	 * @param ExecutionParams 游戏效果自定义执行参数，包含计算伤害所需的各种输入数据
	 * @param OutExecutionOutput 执行输出，通过此参数返回计算结果
	 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
