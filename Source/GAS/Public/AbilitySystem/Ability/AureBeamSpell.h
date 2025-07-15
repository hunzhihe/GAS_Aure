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

	/**
	 * 拾取闪电链命中的第一个目标
	 * @param BeamTargetLocation 鼠标点击目标位置
	 * @note 有可能中间会被阻挡，拾取的目标不是鼠标选中的目标
	 */
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	/**
	 * 通过技能命中目标获取扩散的敌人目标
	 * @param OutAdditionalTargets 返回获取到的最近的目标数组
	 */
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);


	/**
	 * 鼠标命中的敌人死亡处理
	 * @param DeadActor 命中敌人
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);

	/**
	 * 额外的敌人死亡处理
	 * @param DeadActor 额外敌人 
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);


	/**
	 * 技能结束时调用(取消绑定)
	 */
	UFUNCTION(BlueprintCallable)
	void OnEndAbility(TArray<AActor*> AdditionalTargets);

	UFUNCTION(BlueprintCallable)
	float GetScalableFloatValue(FScalableFloat ScalableFloat, int32 Level);

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
