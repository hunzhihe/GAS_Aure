// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AureFireBolt.h"

#include "AureGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AureProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAureFireBolt::GetDescription(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"火球术");
}

FString UAureFireBolt::GetNextLevelDescription(int32 Level)
{
	return GetDescriptionAtLevel(Level, L"下一等级");
}

FString UAureFireBolt::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	const int32 Damage = GetDamageByDamageType(Level, FAureGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>等级：</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		ManaCost,
		FMath::Min(Level, MaxNumProjectiles),
		Damage);
	
}

void UAureFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
	const FName SocketName, const bool bOverridePitch, const float PitchOverride, AActor* HomingTarget)
{
	//判断此函数是否在服务器运行
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
	{
		return;
	}

	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		//限制产生火球的最大数量
	    int32 NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
		
		//获取火球释放位置
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		//方向转为旋转
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		//覆写发射角度
		if (bOverridePitch) Rotation.Pitch = PitchOverride;

		//获取朝向向量
		const FVector Forward = Rotation.Vector();

		//根据函数获取到所有生成的转向
		TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward,FVector ::UpVector, ProjectileSpread, NumProjectiles);
		
		//遍历所有朝向，并生成火球术
	    for (FRotator& Rot : Rotations)
	    {
			// 初始化一个变换对象，用于设置生成物体的位置和旋转
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation); // 设置生成位置
			SpawnTransform.SetRotation(Rot.Quaternion()); // 设置生成旋转
			
			// 使用延迟生成方式生成一个AAureProjectile对象
			// 这种方式可以避免在生成过程中出现的碰撞检测问题
			AAureProjectile* FireBolt = GetWorld()->SpawnActorDeferred<AAureProjectile>(
			    ProjectileClass, // 生成的物体类
			    SpawnTransform, // 生成的变换设置
			    GetOwningActorFromActorInfo(), // 获取拥有者信息
			    Cast<APawn>(GetAvatarActorFromActorInfo()), // 获取玩家信息
			    ESpawnActorCollisionHandlingMethod::AlwaysSpawn); // 总是生成，忽略碰撞
			
			// 初始化生成的物体的伤害效果参数
			FireBolt->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(HomingTarget);

	    	//根据目标类型设置HomingTargetComponent，此内容必须在飞弹被生成后设置
	    	if(HomingTarget && HomingTarget->Implements<UCombatInterface>())
	    	{
	    		//设置攻击的位置为攻击对象的根位置
	    		FireBolt->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
	    	}
	    	else
	    	{
	    		//如果没有获取到攻击目标，则创建一个可销毁的并应用
	    		FireBolt->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
	    		FireBolt->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation); //设置组件位置
	    		FireBolt->ProjectileMovement->HomingTargetComponent = FireBolt->HomingTargetSceneComponent;
	    	}
	    	//设置飞弹朝向目标时的加速度
	    	FireBolt->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
	    	FireBolt->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles; //设置为true，飞弹将加速飞向攻击目标
	    	
	    	
			// 完成物体的生成过程
			FireBolt->FinishSpawning(SpawnTransform);
	    }
	}
}
