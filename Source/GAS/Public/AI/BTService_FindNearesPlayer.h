// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindNearesPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UBTService_FindNearesPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()
protected:
	//周期性重复执行，用于更新数据使用
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 目标选择器，用于在蓝图中只读地选择和编辑跟随的目标
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FBlackboardKeySelector TargetToFollowSelector;
	
	// 与目标距离的选择器，用于在蓝图中只读地选择和编辑与目标的距离
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		FBlackboardKeySelector DistanceToTargetSelector;
};
