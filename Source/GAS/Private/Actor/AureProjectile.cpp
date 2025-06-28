// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AureProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/GAS.h"

// Sets default values
AAureProjectile::AAureProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	

// 创建一个名为"Sphere"的USphereComponent组件，并将其设置为根组件
Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
SetRootComponent(Sphere);

// 设置球体组件的碰撞类型为Projectile
Sphere->SetCollisionObjectType(ECC_Projectile);
// 启用仅查询模式的碰撞检测，意味着该组件不会受到物理模拟的影响
Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 设置球体组件对所有碰撞通道的默认响应为忽略
Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
// 设置球体组件对动态世界的碰撞响应为重叠
Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
// 设置球体组件对静态世界的碰撞响应为重叠
Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
// 设置球体组件对Pawn的碰撞响应为重叠
Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

// 创建一个名为"ProjectileMovement"的UProjectileMovementComponent组件
ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
// 设置初始速度为550单位/秒
ProjectileMovement->InitialSpeed = 550.f;
// 设置最大速度为550单位/秒，与初始速度相同，限制速度不变
ProjectileMovement->MaxSpeed = 550.f;
// 设置重力影响为0，意味着投射物不会受到重力影响
ProjectileMovement->ProjectileGravityScale = 0.f;

}

void AAureProjectile::BeginPlay()
{
Super::BeginPlay();

// 设置生命期，限制项目在游戏中存在的时间
SetLifeSpan(LifeSpan);

// 启用运动复制，确保在网络游戏中项目能正确同步位置
SetReplicateMovement(true);

// 当Sphere组件开始与其他组件重叠时，绑定并调用OnSphereOverlap方法
Sphere->OnComponentBeginOverlap.AddDynamic(
    this, &AAureProjectile::OnSphereOverlap);

	//添加循环音效，并且附加到跟组件上
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(
		LoopingSound, GetRootComponent());
	
}

void AAureProjectile::OnHit()
{
	// 在当前演员的位置播放撞击声音
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	// 在当前演员的位置生成撞击特效
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	
	// 如果存在循环声音组件，则停止并销毁该组件
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	
	// 设置撞击标志为真
	bHit = true;
}

void AAureProjectile::Destroyed()
{
	//销毁循环音效
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	// 如果没有命中目标且没有权限，则播放撞击声音
	if (!bHit && !HasAuthority())
	{
	   OnHit();
	}
	
	Super::Destroyed();
}

void AAureProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (!IsValidOverlap(OtherActor)) return;
	if (!bHit) OnHit();

	//重叠后销毁
	if (HasAuthority())
	{
		// 对目标应用伤害
		if (UAbilitySystemComponent* TargetASC  = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

bool AAureProjectile::IsValidOverlap(AActor* OtherActor)
{
	// 检查伤害效果的来源能力系统组件是否为空，如果为空则返回false，表示不产生伤害效果
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr) return false;
	
	// 获取来源能力系统组件的化身演员，即伤害效果的来源实体
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	
	// 检查伤害来源实体是否与目标实体相同，如果相同则返回false，表示不对自己产生伤害效果
	if (SourceAvatarActor == OtherActor) return false;
	
	// 使用自定义的库函数检查伤害来源实体与目标实体是否为非友军，如果不是非友军则返回false，表示不产生伤害效果
	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return false;
	
	// 所有检查通过，返回true，表示产生伤害效果
	return true;
}



