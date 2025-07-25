// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LocalScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

//用于切换存档显示的用户控件的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitecherIndex);
/**
 * 
 */
UCLASS()
class GAS_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	//切换存档显示的用户控件的委托
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	void InitializeSlot() const;

	//存储玩家关卡出生位置的标签
	UPROPERTY()
	FName PlayerStartTag;

	//当前视图模型的索引，对应存档的索引
	UPROPERTY()
	int32 SlotIndex;

	//当前进入加载存档界面时，此存档应该显示的用户控件界面。
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> LoadSlotStatus;

	void SetSlotName(const FString& InSlotName);
	FString GetSlotName() const { return SlotName; };

	void SetPlayerName(const FString& InPlayerName);
	FString GetPlayerName() const { return PlayerName; };

	void SetMapName(const FString& InMapName);
	FString GetMapName() const { return MapName; };

	void SetPlayerLevel(const int32 InPlayerLevel);
	int32 GetPlayerLevel() const { return PlayerLevel; };
private:
	
	//用户控件的名称
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString SlotName;

	//用户设置的角色名称
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString PlayerName;

	//存档对应的地图名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess));
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess));
	int32 PlayerLevel;


};
