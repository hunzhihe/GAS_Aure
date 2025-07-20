// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureDamageGameplayAbility.h"
#include "Actor/AureFireBall.h"
#include "AureFireBlast.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureFireBlast : public UAureDamageGameplayAbility
{
	GENERATED_BODY()
public:
	//获取火球术技能描述
	virtual FString GetDescription(int32  Level) override;
	virtual FString GetNextLevelDescription(int32  Level) override;

	//获取对应等级的技能描述
	FString GetDescriptionAtLevel(int32 Level,const FString& Title);

	/**
	 * 生成技能所需的火球
	 * @return NumFireBalls个数火球的数组
	 */
	UFUNCTION(BlueprintCallable)
	TArray<AAureFireBall*> SpawnFireBall();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Ability|FireBlast")
	int32 NumFireBalls = 12;

private:
	//生成火球使用的类
	UPROPERTY(EditDefaultsOnly, Category="FireBlast") 
	TSubclassOf<AAureFireBall> FireBallClass;
};
