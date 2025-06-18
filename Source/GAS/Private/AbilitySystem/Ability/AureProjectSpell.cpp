// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureProjectSpell.h"

#include "Actor/AureProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAureProjectSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//UKismetSystemLibrary::PrintString(this, TEXT("Activate Ability"), true, true, FColor::Red, 5.f);
}

// 函数注释：发射 projectile（投射物）
// 参数:
// - ProjectileTargetLocation: 投射物的目标位置
// - SocketTag: 用于标识投射物发射点的标签
// - bOverridePitch: 是否覆盖投射物的俯仰角
// - PitchOverride: 覆盖的俯仰角值
void UAureProjectSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	bool bOverridePitch, float PitchOverride)
{
    // 检查是否为服务器，只有服务器有权限发射投射物
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

    // 获取投射物发射点的位置
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag);
	
    // 计算投射物的初始旋转方向
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	
    // 如果需要覆盖俯仰角，则设置俯仰角为指定的值
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	
    // 设置投射物的出生变换（位置和旋转）
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
    // 在世界中延迟生成投射物实例
	AAureProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAureProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
    // 完成投射物的生成
	Projectile->FinishSpawning(SpawnTransform);
}

