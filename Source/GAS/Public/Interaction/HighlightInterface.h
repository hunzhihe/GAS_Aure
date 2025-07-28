// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAS_API IHighlightInterface
{
	GENERATED_BODY()

	 
public:
	// 定义一个蓝图原生事件，用于高亮显示一个演员（Actor）
	UFUNCTION(BlueprintNativeEvent)
	void HighlightActor();
	
	// 定义一个蓝图原生事件，用于取消高亮显示一个演员（Actor）
	UFUNCTION(BlueprintNativeEvent)
	void UnHighlightActor();

	UFUNCTION(BlueprintNativeEvent)
	void SetMoveToLocation(FVector& OutDestination);
};
