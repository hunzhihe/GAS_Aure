// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAS_API ISaveInterface
{
	GENERATED_BODY()
public:
	//设置Actor是否需要修改位置变换(比如已经开启过的门，下次加载存档时不再关闭)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldLoadTransform();

	//在存档读取完数据后，调用更新Actor
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void LoadActor();
};
