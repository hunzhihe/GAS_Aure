// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	//蓝图事件 设置伤害文字
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage,bool IsBlockedHit, bool IsCriticalHit); 
};
