// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AureGameModeBase.h"

#include "Checkpoints/CheckPoint.h"
#include "Game/AureGameInstance.h"
#include "Game/LocalScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MVVM/MVVM_LoadSlot.h"

AActor* AAureGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const UAureGameInstance* RPGGameInstance = Cast<UAureGameInstance>(GetGameInstance());
	
	//获取关卡里的所有PlayerStart实例
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	//已激活的检查点高亮
	HightLightEnableCheckpoints(Actors);
	
	if(Actors.Num() > 0)
	{
		//获取到第一个实例对象
		AActor* SelectedActor = Actors[0];
		for(AActor* Actor : Actors)
		{
			if(APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				//判断PlayerStart的Tag设置是否为指定的Tag
				if(PlayerStart->PlayerStartTag == RPGGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void AAureGameModeBase::TravelToMap(const UMVVM_LoadSlot* Slot)
{
	const FString SlotName = Slot->GetSlotName();
	const int32 SlotIndex = Slot->SlotIndex;
	
	//打开地图
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

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
	LocalScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LocalScreenSaveGame->SaveSlotStatus = ESaveSlotStatus::Taken;
	LocalScreenSaveGame->SlotIndex = SlotIndex;
	LocalScreenSaveGame->SlotName = LoadSlot->GetSlotName();
	LocalScreenSaveGame->MapName = LoadSlot->GetMapName();
	LocalScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

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

void AAureGameModeBase::DeleteSlotData(const FString& SlotName, int32 SlotIndex)
{
	//检查是否有对应名称的存档
	if(UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		//删除已保存的存档
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULocalScreenSaveGame* AAureGameModeBase::RetrieveInGameSaveData() const
{
	const UAureGameInstance* RPGGameInstance = Cast<UAureGameInstance>(GetGameInstance());

	//从游戏实例获取到存档名称和索引
	const FString InGameLoadSlotName = RPGGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = RPGGameInstance->LoadSlotIndex;

	//获取已保存的存档数据
	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
	
}

void AAureGameModeBase::SaveInGameProgressData(ULocalScreenSaveGame* SaveObject) const
{
	UAureGameInstance* RPGGameInstance = Cast<UAureGameInstance>(GetGameInstance());

	//修改下一次复活的检测点
	RPGGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;

	//从游戏实例获取到存档名称和索引
	const FString InGameLoadSlotName = RPGGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = RPGGameInstance->LoadSlotIndex;

	//保存存档
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
	
}

void AAureGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}

void AAureGameModeBase::HightLightEnableCheckpoints(TArray<AActor*> CheckPoints) const
{
	//获取存档
	ULocalScreenSaveGame* SaveGameData = RetrieveInGameSaveData();
	if (SaveGameData == nullptr)
	{
		return;
	}

	//遍历地图内的所有检查点，如果数组存在，将高亮显示
	for (AActor* Actor : CheckPoints)
	{
		if (ACheckPoint* CheckPoint = Cast<ACheckPoint>(Actor))
		{
			if (SaveGameData->ActivatedPlayerStartTags.Contains(CheckPoint->PlayerStartTag))
			{
				CheckPoint->HandleGlowEffects();
			}
		}
	}
}
