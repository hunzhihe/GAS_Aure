// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AureAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class GAS_API AAureAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAureAIController();

protected:
	//行为树组件
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
