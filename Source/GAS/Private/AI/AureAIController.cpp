// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AureAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAureAIController::AAureAIController()
{
	// 创建并初始化黑板组件，用于AI的决策和数据存储
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	// 确保黑板组件成功创建，此时应进行初始化检查
	check(Blackboard);
	
	// 创建并初始化行为树组件，用于定义AI的行为逻辑
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	// 确保行为树组件成功创建，此时应进行初始化检查
	check(BehaviorTreeComponent);
}
