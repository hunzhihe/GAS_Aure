// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AureGameModeBase.h"

#include "EngineUtils.h"
#include "Checkpoints/CheckPoint.h"
#include "Game/AureGameInstance.h"
#include "Game/LocalScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "GAS/AureLogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/MVVM/MVVM_LoadSlot.h"

AActor* AAureGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	const UAureGameInstance* RPGGameInstance = Cast<UAureGameInstance>(GetGameInstance());
	
	//获取关卡里的所有PlayerStart实例
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	//已激活的检查点高亮
	//HightLightEnableCheckpoints(Actors);
	
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
	USaveGame* SaveGame = nullptr;
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

void AAureGameModeBase::SaveWorldState(UWorld* World,const FString& DestinationMapAssetName) const
{
	//获取关卡名称
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	//获取到游戏实例
	UAureGameInstance* AureGameInstance = Cast<UAureGameInstance>(GetGameInstance());
    check(AureGameInstance);
	
	//获取存档
	if (ULocalScreenSaveGame* SaveGameData = GetSaveSlotData(AureGameInstance->LoadSlotName, AureGameInstance->LoadSlotIndex))
	{
        //修改存档地图资源名称和地图名称
        if (DestinationMapAssetName != FString(""))
        {
	        SaveGameData->MapAssetName = DestinationMapAssetName;
        	SaveGameData->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
        }
		//如果存档不包含对应关卡内容，将创建一个对应的数据结构存储
		if (!SaveGameData->HasMap(WorldName))
		{
			FSavedMap NewSaveMap;
			NewSaveMap.MapAssetName = WorldName;
			SaveGameData->SavedMaps.Add(NewSaveMap);
		}
		//获取对应的存档关卡数据结构体
		FSavedMap SaveMap = SaveGameData->GetSavedMapWithMapName(WorldName);
		//清空关卡内的所有存档数据
		SaveMap.SavedActors.Empty();

		//利用迭代器遍历场景中的每一个Actor,将需要保存Actor数据保存到结构体中
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			//判断Actor是否存在，并判断Actor是否需要存储
			if(!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			//创建存储结构体
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			//创建一个 FMemoryWriter，用于将数据写入SavedActor.Bytes
			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			//创建一个序列化器，将对象的成员以名称和值的形式保存到 MemoryWriter。
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true; //设置序列化方式为保存到存档的模式

			//将Actor所需要保存的数据写入到Archive，Archive将把数据存储到SavedActor.Bytes
			Actor->Serialize(Archive);

			SaveMap.SavedActors.AddUnique(SavedActor);
		}

		//找到对应的名称的结构体，将数据存储到存档对象内
		for(FSavedMap& MapToReplace : SaveGameData->SavedMaps)
		{
			if(MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SaveMap;
			}
		}

		//保存存档
		UGameplayStatics::SaveGameToSlot(SaveGameData, AureGameInstance->LoadSlotName, AureGameInstance->LoadSlotIndex);
	}
	
}

void AAureGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAureGameInstance* AureGameInstance = Cast<UAureGameInstance>(GetGameInstance());
	check(AureGameInstance);

	//检查是否有对应名称的存档
	if (UGameplayStatics::DoesSaveGameExist(AureGameInstance->LoadSlotName, AureGameInstance->LoadSlotIndex))
	{
        //读取存档
		ULocalScreenSaveGame* SaveGame = Cast<ULocalScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AureGameInstance->LoadSlotName, AureGameInstance->LoadSlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAure, Error, TEXT("加载存档失败"));
			return;
		}
		//存档是否有对应关卡数据
        if (SaveGame->HasMap(WorldName))
        {
        	//遍历场景内的Actor,寻找存档内对应的数据并应用到场景
        	for (FActorIterator It(World); It; ++It)
        	{
        		AActor* Actor = *It;

        		if (!Actor->Implements<USaveInterface>()) continue;

        		//遍历存档里对应关卡的所有actor数据
        		for (FSavedActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
        		{
        			//查找到对应的actor的存档数据
        			if (SavedActor.ActorName == Actor->GetFName())
        			{
        				if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
        				{
        					Actor->SetActorTransform(SavedActor.Transform);
        				}
        				//反序列化，创建一个FMemoryReader实例用于从二进制数据中读取内容
        				FMemoryReader MemoryReader(SavedActor.Bytes);

        				//FObjectAndNameAsStringProxyArchive 代理类，用于序列化和反序列化对象的属性 true：表示允许使用字符串形式的对象和属性名称（便于调试和可读性）。
        				FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
        				//指定反序列化是用于加载存档数据。
        				Archive.ArIsSaveGame = true;
        				//执行反序列化，将二进制数据设置到actor属性上
        				Actor->Serialize(Archive);
        				
        				//修改Actor上的属性后，调用函数更新Actor的显示
        				ISaveInterface::Execute_LoadActor(Actor);
        			}
        		}
        	}
        }
		
	}
}

FString AAureGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	/**
	 * 根据地图资产名称查找对应的地图键值
	 * @param Maps 包含地图映射关系的容器
	 * @param MapAssetName 要查找的地图资产名称
	 * @return 返回匹配的地图键值，如果未找到则返回空字符串
	 */
		for (auto& Map : Maps)
		{
			// 查找与指定资产名称匹配的地图项
			if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
			{
				return Map.Key;
			}
		}
		return FString();
}

void AAureGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}

// void AAureGameModeBase::HightLightEnableCheckpoints(TArray<AActor*> CheckPoints) const
// {
// 	//获取存档
// 	ULocalScreenSaveGame* SaveGameData = RetrieveInGameSaveData();
// 	if (SaveGameData == nullptr)
// 	{
// 		return;
// 	}
//
// 	//遍历地图内的所有检查点，如果数组存在，将高亮显示
// 	for (AActor* Actor : CheckPoints)
// 	{
// 		if (ACheckPoint* CheckPoint = Cast<ACheckPoint>(Actor))
// 		{
// 			if (SaveGameData->ActivatedPlayerStartTags.Contains(CheckPoint->PlayerStartTag))
// 			{
// 				CheckPoint->HandleGlowEffects();
// 			}
// 		}
// 	}
// }
