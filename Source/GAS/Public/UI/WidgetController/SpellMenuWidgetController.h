// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureGameplayTags.h"
#include "UI/WidgetController/AureWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

//在技能面板选中的技能的标签结构体
USTRUCT(BlueprintType)
struct FSelectedAbility
{
	GENERATED_BODY()
	//技能标签
	FGameplayTag Ability = FGameplayTag();
	//技能状态标签
	FGameplayTag AbilityStatus = FGameplayTag();
	//技能等级
	int32 Level = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellDescriptionSignature, FString, SpellDescription, FString, SpellNextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature,bool, bSpendPointsEnabled,bool,bEquipEnabled,bool,bDemotionPointsEnabled,int32,AbilityLevel);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_API USpellMenuWidgetController : public UAureWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	//广播当前技能按钮升降级按钮状态和可分配状态
	void BroadcastSpellGlobeSelected();

	//设置选中技能默认值
	FSelectedAbility SelectedAbility = {FAureGameplayTags::Get().Abilities_None,FAureGameplayTags::Get().Abilities_Status_Locked};

	//保存当前技能可分配点数
	int32 CurrentSpellPoints = 0;
	
	//技能点变动委托
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointChanged;

	//选择技能图标，升级和装备按钮的变动回调
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedSignature;

	//技能描述回调
	UPROPERTY(BlueprintAssignable)
	FSpellDescriptionSignature SpellDescriptionSignature;


	//技能按钮选中调用函数
	UFUNCTION(BlueprintCallable)
	FGameplayTag SpellGlobeSelected(const FGameplayTag& AbilityTag);

	//升级按钮调用函数
	UFUNCTION(BlueprintCallable)
    void SpendPointButtonPressed(const FGameplayTag& AbilityTag);
	//降级按钮调用函数
	UFUNCTION(BlueprintCallable)
	void DemotionPointButtonPressed(const FGameplayTag& AbilityTag);

	//再次点击，取消按钮选中处理
	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();

	//装配按钮点击事件
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	virtual void ClearAllDelegate() override;
private:

	//通过技能状态标签和可分配技能点数来设置技能是否可以装配，技能是否可以升级
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool HasSpellPoints,bool& bShouldEnableSpellPoints, bool& bShouldEnableEquip, bool& bShouldDemotionPoints);
};
