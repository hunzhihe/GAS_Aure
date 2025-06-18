// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AureProjectile.h"

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

// Called when the game starts or when spawned
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
}

void AAureProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		PlayImpact();
	}
	Super::Destroyed();
}

void AAureProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	PlayImpact();

	//重叠后销毁
	if (HasAuthority())
	{
		Destroy();
	}
	else
	{
		bHit = true;
	}


	
}



