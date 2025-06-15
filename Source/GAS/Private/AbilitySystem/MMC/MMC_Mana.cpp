// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/MMC_Mana.h"

#include "AbilitySystem/AureAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_Mana::UMMC_Mana()
{
	// 定义智力属性的捕捉配置
	IntelligenceDef.AttributeToCapture = UAureAttributeSet::GetIntelligenceAttribute();
	// 设置智力属性的来源为效果的来源
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	// 配置智力属性不进行快照处理
	IntelligenceDef.bSnapshot = false;
	
	// 将智力属性添加到需要捕捉的属性列表中
	RelevantAttributesToCapture.Add(IntelligenceDef);
	
}

float UMMC_Mana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 获取源和目标的标签集合，用于后续的评估参数配置
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	//初始化评估参数结构体，用于属性计算时传递源和目标的标签信息
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// 初始化体力值，并确保其非负
	float Intelligence = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluationParameters, Intelligence);
	Intelligence = FMath::Max<float>(Intelligence, 0.f);
	
	// 默认玩家等级为1，根据源对象是否实现战斗接口来获取实际玩家等级
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	// 根据体力值和玩家等级计算并返回最终结果
	return 50.f + 1.5f * Intelligence + 10.f * PlayerLevel;
	
}
