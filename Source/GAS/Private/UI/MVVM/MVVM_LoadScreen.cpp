// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MVVM/MVVM_LoadScreen.h"

#include "Game/AureGameInstance.h"
#include "Game/AureGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MVVM/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_0->SetSlotName(TEXT("LoadSlot_0"));
	LoadSlots.Add(0,LoadSlot_0);

	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_1->SetSlotName(TEXT("LoadSlot_1"));
	LoadSlots.Add(1,LoadSlot_1);

	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_2->SetSlotName(TEXT("LoadSlot_2"));
	LoadSlots.Add(2,LoadSlot_2);

	LoadSlot_3 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_3->SetSlotName(TEXT("LoadSlot_3"));
	LoadSlots.Add(3,LoadSlot_3);
}

void UMVVM_LoadScreen::LoadData()
{
	//获取到加载存档界面的GM
	AAureGameModeBase* GM = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	//遍历映射，获取对应存档
	for (const TTuple<int32, UMVVM_LoadSlot*> Slot : LoadSlots)
	{
		// 从指定的存档中加载游戏数据
		ULocalScreenSaveGame* SaveGame = GM->GetSaveSlotData(Slot.Value->GetSlotName(), Slot.Key);
		
		// 获取存档数据
		const FString PlayerName = SaveGame->PlayerName;
		const TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveGame->SaveSlotStatus;
		
		// 设置存档视图模型数据
		Slot.Value->SetPlayerName(PlayerName);
		Slot.Value->SetMapName(SaveGame->MapName);
		Slot.Value->LoadSlotStatus = SaveSlotStatus;
		Slot.Value->PlayerStartTag = SaveGame->PlayerStartTag;
		Slot.Value->SetPlayerLevel(SaveGame->PlayerLevel); 
		
		//调用视图模型初始化
		Slot.Value->InitializeSlot();
		
	}
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AAureGameModeBase* GM = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	//修改MVVM上存储的角色名称
	LoadSlots[Slot]->SetPlayerName(EnterName);
	//修改地图名称
	LoadSlots[Slot]->SetMapName(GM->DefaultMapName);
	//修改进入界面为加载界面
	LoadSlots[Slot]->LoadSlotStatus = Taken;
	//存储关卡生成点
	LoadSlots[Slot]->PlayerStartTag = GM->DefaultPlayerStartTag;
    //修改角色等级
	LoadSlots[Slot]->SetPlayerLevel(1);
	
	
	//保存数据
	GM->SaveSlotData(LoadSlots[Slot], Slot);
	//初始化
	LoadSlots[Slot]->InitializeSlot();
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	AAureGameModeBase* GM = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	//设置全局数据，方便后续使用
	UAureGameInstance* AureGameInstance = Cast<UAureGameInstance>(GM->GetGameInstance());
	AureGameInstance->LoadSlotName = LoadSlots[Slot]->GetSlotName();
	AureGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	AureGameInstance->PlayerStartTag = LoadSlots[Slot]->PlayerStartTag;
	

	//进入场景
	GM->TravelToMap(LoadSlots[Slot]);
}

void UMVVM_LoadScreen::DeleteButtonPressed(int32 Slot)
{
	//删除存档
	const AAureGameModeBase* GM = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	GM->DeleteSlotData(LoadSlots[Slot]->GetSlotName(), Slot);

	//修改存档控件显示
	LoadSlots[Slot]->LoadSlotStatus = Vacant;
	LoadSlots[Slot]->InitializeSlot();
}

void UMVVM_LoadScreen::SetWidgetName(const FString& InSlotName)
{
	//宏定义，用于设置属性值并且通知变更
	if (UE_MVVM_SET_PROPERTY_VALUE(WidgetName, InSlotName))
	{
		//通过宏调用其它函数的广播
		// UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(setHealth); 
	}
}
