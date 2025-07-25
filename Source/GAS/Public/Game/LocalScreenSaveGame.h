// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LocalScreenSaveGame.generated.h"

//当前存档显示的用户控件枚举
UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};


/**
 * 
 */
UCLASS()
class GAS_API ULocalScreenSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:

	//存档名称
	UPROPERTY()
	FString SlotName = FString();

	//存档索引
	UPROPERTY()
	int32 SlotIndex = 0;

	//玩家姓名
	UPROPERTY()
	FString PlayerName = FString("Default Name");

	//地图名称
	UPROPERTY()
	FString MapName = FString();

	//存储玩家关卡出生位置的标签
	UPROPERTY()
	FName PlayerStartTag;

	//当前存档进入存档界面时，默认显示的用户界面
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;

	//是否是第一次进入存档
	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	//当前已经激活的监测点
	UPROPERTY()
	TArray<FName> ActivatedPlayerStartTags = TArray<FName>();

	/************************** 玩家属性 **************************/

	UPROPERTY()
	int32 PlayerLevel = 1;
	
	//经验值
	UPROPERTY()
	int32 XP = 0;

	//可分配技能点
	UPROPERTY()
	int32 SpellPoints = 0;

	//可分配属性点
	UPROPERTY()
	int32 AttributePoints = 0;

	/************************** 主要属性 **************************/
	
	//力量
	UPROPERTY()
	float Strength = 0;

	//智力
	UPROPERTY()
	float Intelligence = 0;

	//韧性
	UPROPERTY()
	float Resilience = 0;

	//体力
	UPROPERTY()
	float Vigor = 0;
	
};
