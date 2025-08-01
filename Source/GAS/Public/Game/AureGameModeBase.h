// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AureGameModeBase.generated.h"

class ULootTiers;
class UMVVM_LoadSlot;
class ULocalScreenSaveGame;
class USaveGame;
class UCharacterClassInfo;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class GAS_API AAureGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	//初始地图名称
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	//游戏初始地图
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	//角色切换关卡后默认生成位置的PlayerStart的标签
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;


	//覆写父类的选择PlayerStart函数，修改为可以通过Tag获取生成位置
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	
	//地图名称和地图的映射
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	//跳转地图
	void TravelToMap(const UMVVM_LoadSlot* Slot);

	/**
	 * 玩家死亡
	 * @param DeadCharacter 死亡玩家
	 */
	void PlayerDied(const ACharacter* DeadCharacter) const;

	/**
    * 创建新存档
    * @param LoadSlot 需要保存的视图模型示例
    * @param SlotIndex 存档索引
    */
	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const;

	/**
	 * 获取保存的存档
	 * @param SlotName 存档名称（每个存档名称固定）
	 * @param SlotIndex 存档索引
	 * @return 
	 */
	ULocalScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	//存档使用的数据结构
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LocalScreenSaveGameClass;

	/**
	 * 删除存档
	 * @param SlotName 需要删除存档对应的视图模型实例名称
	 * @param SlotIndex 存档索引
	 */
	static void DeleteSlotData(const FString& SlotName, int32 SlotIndex);

	//获取到当前游戏进行中所使用的存档数据
	ULocalScreenSaveGame* RetrieveInGameSaveData() const;

	/**
	 * 保存游戏中的进度
	 * @param SaveObject 需要保存的数据
	 */
	void SaveInGameProgressData(ULocalScreenSaveGame* SaveObject) const;
	

	//保存关卡中的状态到当前存档中 World当前所处事件 DestinationMapAssetName 目标地图资源名称
	void SaveWorldState(UWorld* World,const FString& DestinationMapAssetName = FString("")) const;
	//从存档中加载当前关卡的状态
	void LoadWorldState(UWorld* World) const;

    //根据地图名称获取地图资源路径
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

	
protected:
	virtual void BeginPlay() override;

private:
	//高亮已经激活的检查点
	//void HightLightEnableCheckpoints(TArray<AActor*> CheckPoints) const;
};
