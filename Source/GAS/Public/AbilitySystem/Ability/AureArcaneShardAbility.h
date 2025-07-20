// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureDamageGameplayAbility.h"
#include "AureArcaneShardAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureArcaneShardAbility : public UAureDamageGameplayAbility
{
	GENERATED_BODY()

public:
	//获取火球术技能描述
	virtual FString GetDescription(int32  Level) override;
	virtual FString GetNextLevelDescription(int32  Level) override;

	//获取对应等级的技能描述
	FString GetDescriptionAtLevel(int32 Level,const FString& Title);

protected:
	//技能最多可生成的奥数爆发的个数
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 MaxNumShards = 11;
};
