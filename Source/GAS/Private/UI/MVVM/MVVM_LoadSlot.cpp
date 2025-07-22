// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MVVM/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot() const
{
	//从枚举获取到对应的索引
	const int32 WidgetSwitcherIndex = LoadSlotStatus.GetValue();
	//广播切换显示
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetSlotName(const FString& InSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SlotName, InSlotName);
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}
