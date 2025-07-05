// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

//升级信息结构体
USTRUCT(BlueprintType)
struct FAureLevelUpInfo
{
	GENERATED_BODY()

	//升到该等级所需经验值
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 1;

	//奖励属性点数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttributePointAward = 1;
	//奖励技能点数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpellPointAward = 1;
};



UCLASS()
class GAS_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	//当前所有等级的升级信息
	UPROPERTY(EditDefaultsOnly)
	TArray<FAureLevelUpInfo> LevelUpInformation;

	//根据XP值，返回对应的等级
	int32 FindLevelForXP(int32 XP) const;
};
