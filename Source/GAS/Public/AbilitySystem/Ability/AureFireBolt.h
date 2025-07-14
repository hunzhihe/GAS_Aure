// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureProjectSpell.h"
#include "AureFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureFireBolt : public UAureProjectSpell
{
	GENERATED_BODY()
public:
	//获取火球术技能描述
	virtual FString GetDescription(int32  Level) override;
	virtual FString GetNextLevelDescription(int32  Level) override;

	//获取对应等级的技能描述
	FString GetDescriptionAtLevel(int32 Level,const FString& Title);

	//生成多端火球的函数
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
		const FName SocketName, const bool bOverridePitch = false, const float PitchOverride = 0.f,AActor* HomingTarget =  nullptr);

protected:
	UPROPERTY(EditDefaultsOnly,Category="FireBolt")
    float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly,Category="FireBolt")
	int32 MaxNumProjectiles = 5;
	
	//朝向目标移动的最小加速度
	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	float HomingAccelerationMin = 1000.f;
	//朝向目标移动的最大加速度
	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	float HomingAccelerationMax = 3200.f;
	//设置生成的飞弹是否需要朝向目标飞行
	UPROPERTY(EditDefaultsOnly,Category = "FireBolt")
	bool bLaunchHomingProjectiles = true;
};
