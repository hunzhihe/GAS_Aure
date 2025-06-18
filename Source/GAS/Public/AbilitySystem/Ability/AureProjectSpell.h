// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureGameplayAbility.h"
#include "AureProjectSpell.generated.h"

class AAureProjectile;
/**
 * 
 */
UCLASS()
class GAS_API UAureProjectSpell : public UAureGameplayAbility
{
	GENERATED_BODY()
protected:
/**
 * 激活游戏玩法能力的虚拟函数
 * 
 * @param Handle 能力的唯一句柄，用于标识和管理能力
 * @param ActorInfo 指向演员信息的指针，包含执行能力的演员的相关信息
 * @param ActivationInfo 能力激活信息，包括激活原因和激活者等信息
 * @param TriggerEventData 指向触发事件数据的指针，可能包含触发能力的事件的附加信息
 */
virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
/**
 * 发射射弹函数
 * 
 * @param ProjectileTargetLocation 射弹的目标位置，指定射弹将要飞行到的地点
 * @param SocketTag 套接字标签，用于指定射弹应该从哪个具体的点（例如，角色的左手或右手）发射
 * @param bOverridePitch 是否覆盖俯仰角的标志，如果为true，则俯仰角将被PitchOverride参数值覆盖
 * @param PitchOverride 覆盖的俯仰角值，当bOverridePitch为true时生效，用于精确指定射弹的发射角度
 */
UFUNCTION(BlueprintCallable, Category = "Projectile")
void SpawnProjectile(const FVector& ProjectileTargetLocation,
	const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverride = 0.f);

	

	//火球类
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAureProjectile> ProjectileClass;
};
