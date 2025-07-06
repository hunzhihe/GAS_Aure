// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAS_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    // 获取等级
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;

	// 获取经验值
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	// 获取属性点奖励
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const;


	// 获取技能点奖励
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const;

	//增加经验值
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 IxXP);

	//增加等级
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);

	//增加属性点
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	//获取属性点
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	//增加技能点
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);

	//获取技能点
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	//等级提升
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	
};
