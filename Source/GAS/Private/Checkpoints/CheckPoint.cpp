// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoints/CheckPoint.h"
#include "GameFramework/PlayerStart.h"
#include "Components/SphereComponent.h"
#include "Game/AureGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckPoint::ACheckPoint(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	//关闭帧更新
	PrimaryActorTick.bCanEverTick = false;

	//创建检测点显示模型
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //设置查询并产生物理
	CheckpointMesh->SetCollisionResponseToChannels(ECR_Block); //设置阻挡所有物体与其重叠
	
	//设置自定义深度值
	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilOverride);
	CheckpointMesh->MarkRenderStateDirty();//该函数作用：修改了材质，顶点，渲染可见性后，可以将其延后到帧结束，提交数据，下一帧更新，提升性能。
	
	//设置球碰撞体
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); //设置其忽略所有碰撞检测
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //设置其与Pawn类型物体产生重叠事件

    //自动移动组件
	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();

	//绑定重叠事件
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnSphereOverlap);
}

void ACheckPoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if(OtherActor->ActorHasTag("Player")) //如果只需要判断是不是玩家角色通过标签判断即可
	if(OtherActor->Implements<UPlayerInterface>())
	{
		//检测点已被激活
		bReached = true;

		if(AAureGameModeBase* AureGameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			//保存场景状态
			AureGameMode->SaveWorldState(GetWorld());
		}
		
		//修改存档当的检测点
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		
		//如果与碰撞体重叠的是
		HandleGlowEffects();
	}
}

void ACheckPoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void ACheckPoint::HighlightActor_Implementation()
{
	if (!bReached)
	{
		CheckpointMesh->SetRenderCustomDepth(true);
	}
}

void ACheckPoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckPoint::LoadActor_Implementation()
{
	if (bReached)
	{
		CheckpointMesh->SetMaterial(0, CheckpointMaterial);
		HandleGlowEffects();
	}
}

void ACheckPoint::HandleGlowEffects()
{
	//取消碰撞检查
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//创建一个新材质实例，修改效果
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(CheckpointMaterial, this);
	check(DynamicMaterialInstance);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	//触发检查点修改材质后的回调
	CheckpointReached(DynamicMaterialInstance); 
}
