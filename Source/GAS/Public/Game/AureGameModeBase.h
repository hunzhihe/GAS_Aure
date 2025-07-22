// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AureGameModeBase.generated.h"

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

	void PlayerDied(ACharacter* DeadCharacter);

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

};
