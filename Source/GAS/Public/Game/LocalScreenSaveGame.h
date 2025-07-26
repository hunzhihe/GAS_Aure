// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
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
//存储技能的相关信息结构体
USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	//需要存储的技能
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	//当前技能的等级
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 0;

	//当前技能的标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	//当前技能的状态标签
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	//当前技能装配到的插槽，如果技能未装配则为空
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityInputTag = FGameplayTag();

	//当前技能的类型（主动技能还是被动技能）
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();
	
};

//保存场景中的Actor结构体
USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	//Actor身上序列号的数据，必须通过UPROPERTY定义过，只在保存存档时使用。
	UPROPERTY()
	TArray<uint8> Bytes;
};

//地图相关数据保存
USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

//重载运算符==，判断左右值（FSaveAbility）是否相等,将通过函数内的值判断是否相等
inline bool operator==(const FSavedAbility& LSA, const FSavedAbility& RSA)
{
	return LSA.AbilityTag.MatchesTagExact(RSA.AbilityTag);
}

//自定义运算符==，如果结构体内的ActorName相同，这代表这两个结构体为相同结构体
inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}
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

	/************************** 技能 **************************/

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	/************************** 场景和地图 **************************/

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	//通过地图名称获取地图数据
	FSavedMap GetSavedMapWithMapName(const FString& InMapName);

	//判断存档是否含有对于地图数据
	bool HasMap(const FString& InMapName);
	
};
