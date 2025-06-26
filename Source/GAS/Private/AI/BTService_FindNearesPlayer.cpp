// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearesPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearesPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 获取AI控制器所控制的Pawn
	APawn* OwningPawn = AIOwner->GetPawn();
	
	// 根据OwningPawn的标签确定目标标签，如果OwningPawn有"Player"标签，则目标标签为"Enemy"，否则为"Player"
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");
	
	// 定义一个数组用于存储具有目标标签的Actor
	TArray<AActor*> ActorsWithTag;
	
	// 获取所有具有目标标签的Actor
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);
	
	// 初始化最近距离为浮点数的最大值，初始化最近的Actor为nullptr
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	
	// 遍历所有具有目标标签的Actor，找到最近的一个
	for (AActor* Actor : ActorsWithTag)
	{
	    // 检查Actor和OwningPawn的有效性
	    if (IsValid(Actor) && IsValid(OwningPawn))
	    {
	        // 计算OwningPawn到当前Actor的距离
	        const float Distance = OwningPawn->GetDistanceTo(Actor);
	        // 如果当前距离小于已记录的最近距离，则更新最近距离和最近的Actor
	        if (Distance < ClosestDistance)
	        {
	            ClosestDistance = Distance;
	            ClosestActor = Actor;
	        }
	    }
	}
	
	// 使用最近的Actor作为目标设置到AI的黑板中
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	// 使用最近的距离设置到AI的黑板中
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
