// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureDamageGameplayAbility.h"
#include "AureBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureBeamSpell : public UAureDamageGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	 * 将鼠标拾取命中信息存储
	 * @param HitResult 在技能中通过TargetDataUnderMouse的task获取到的结果
	 */
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	/**
	 * 设置拥有当前技能的玩家控制器
	 */
	UFUNCTION(BlueprintCallable)
	void StoreOwnerPlayerController();

protected:
	//鼠标拾取位置
	UPROPERTY(BlueprintReadWrite, Category="Beam")
	FVector MouseHitLocation; 

	//鼠标拾取的对象
	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<AActor> MouseHitActor; 

	//拥有当前技能的玩家控制器
	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<APlayerController> OwnerPlayerController; 

	//拥有当前技能的玩家角色
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	//beam技能的技能目标数量
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxNumShockTargets = 5;
};
