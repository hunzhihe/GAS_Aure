// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoints/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AureGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

class AAureGameModeBase;

AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::LoadActor_Implementation()
{
	//加载地图时，不做任何修改
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Actor是否继承玩家接口
	if (OtherActor->Implements<UPlayerInterface>())
	{
		//已激活
		bReached = true;
        //保存场景状态
		if (AAureGameModeBase* AuraGM = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			//传入目标关卡的路径，并保存切换地图的相关信息
			AuraGM->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		}

		//将当前角色的进度保存
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayerStartTag);
        //切换地图
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
