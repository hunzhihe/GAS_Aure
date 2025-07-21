// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API ULoadScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	//蓝图实现初始化函数，主要用于设置MVVM
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BlueprintInitializeWidget();
};
