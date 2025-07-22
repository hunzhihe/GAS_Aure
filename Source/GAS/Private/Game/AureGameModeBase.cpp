// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AureGameModeBase.h"

#include "Game/LocalScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MVVM/MVVM_LoadSlot.h"

void AAureGameModeBase::PlayerDied(ACharacter* DeadCharacter)
{
}

void AAureGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const
{
	//检查是否有对应名称的存档
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetSlotName(), SlotIndex))
	{
		//删除已经保存的存档
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetSlotName(), SlotIndex);
	}
	//创建一个新的存档
	USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(LocalScreenSaveGameClass);
	ULocalScreenSaveGame* LocalScreenSaveGame = Cast<ULocalScreenSaveGame>(SaveGame);

	//设置需要保存的数据
	LocalScreenSaveGame->PlayerName = LoadSlot->GetSlotName();
	LocalScreenSaveGame->SaveSlotStatus = ESaveSlotStatus::Taken;
	LocalScreenSaveGame->SlotIndex = SlotIndex;
	LocalScreenSaveGame->SlotName = LoadSlot->GetSlotName();

	//保存
	UGameplayStatics::SaveGameToSlot(LocalScreenSaveGame, LoadSlot->GetSlotName(), SlotIndex);
}

ULocalScreenSaveGame* AAureGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGame;
	//检查是否有对应名称的存档
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		//获取存档
		SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		//创建一个新的存档
		SaveGame = UGameplayStatics::CreateSaveGameObject(LocalScreenSaveGameClass);
	}

	ULocalScreenSaveGame* LocalScreenSaveGame = Cast<ULocalScreenSaveGame>(SaveGame);
	return LocalScreenSaveGame;
}
