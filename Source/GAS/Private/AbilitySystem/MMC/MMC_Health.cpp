// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/MMC_Health.h"

#include "AbilitySystem/AureAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_Health::UMMC_Health()
{
	VigorDef.AttributeToCapture = UAureAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_Health::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	
	// 获取源和目标的标签集合，用于后续的评估参数配置
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	//初始化评估参数结构体，用于属性计算时传递源和目标的标签信息
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	// 初始化体力值，并确保其非负
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);
	
	// 默认玩家等级为1，根据源对象是否实现战斗接口来获取实际玩家等级
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
	    PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	
	// 根据体力值和玩家等级计算并返回最终结果
	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
