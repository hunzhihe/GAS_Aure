// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureProjectSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AureGameplayTags.h"
#include "Actor/AureProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAureProjectSpell::GetDescription(int32 Level)
{
	//根据等级获取技能伤害
	const int32 ScaledDamage = DamageTypes[FAureGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>火球术</>\n<Small>等级：1</>\n\n<Default>发射 1 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"), ScaledDamage);
	}

	return FString::Printf(TEXT("<Title>火球术</>\n<Small>等级：%i</>\n\n<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),
		Level,
		FMath::Min(Level, 5), ScaledDamage);
	
}

FString UAureProjectSpell::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = DamageTypes[FAureGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level + 1); //根据等级获取技能伤害
	return FString::Printf(TEXT("<Title>下一等级</>\n<Small>等级：%i</>\n\n<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),
		Level, FMath::Min(Level, 5), ScaledDamage);
	
}

void UAureProjectSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//UKismetSystemLibrary::PrintString(this, TEXT("Activate Ability"), true, true, FColor::Red, 5.f);
}

// 生成发射 projectile（投射物）
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
	
 //    // 获取拥有者角色的Ability System Component，以便后续使用
 //    const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningActorFromActorInfo());
 //
	// // 创建一个游戏效果上下文句柄，用于下面创建效果规范句柄FGameplayEffectSpecHandle
	// FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	//      /*填充上下文句柄*/
	// //设置技能
	// EffectContextHandle.SetAbility(this);
	// //设置源
	// EffectContextHandle.AddSourceObject(Projectile);
	// //添加Actor列表
	// TArray<TWeakObjectPtr<AActor>> Actors;
	// Actors.Add(Projectile);
	// EffectContextHandle.AddActors(Actors);
	// //添加命中结果
	// FHitResult HitResult;
	// HitResult.Location = ProjectileTargetLocation;
	// EffectContextHandle.AddHitResult(HitResult);
	// //添加技能触发位置
	// EffectContextHandle.AddOrigin(ProjectileTargetLocation);
	//       /*填充上下文句柄*/
	
 //    // 创建一个游戏效果规范句柄，用于描述即将应用的游戏效果
	// const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),EffectContextHandle);
 //
	// //获取标签单例
	// const FAureGameplayTags GameplayTags = FAureGameplayTags::Get();
 //
	// for(auto& Pair : DamageTypes)
	// {
	// 	//根据等级获取技能伤害
	// 	const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
	// 	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("火球术伤害：%f"), ScaledDamage));
	//
	// 	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
	// }
	

   // 将创建的效果规范句柄赋值给Projectile的DamageEffectParams属性
    Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

	// 完成投射物的生成
	Projectile->FinishSpawning(SpawnTransform);

}