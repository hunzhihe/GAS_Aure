// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AureEnemySpawnVolume.h"

#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

// Sets default values
AAureEnemySpawnVolume::AAureEnemySpawnVolume()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	//仅用于检测
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//设置物体类型
	Box->SetCollisionObjectType(ECC_WorldStatic);
	//忽略所有检测通道
	Box->SetCollisionResponseToChannels(ECR_Ignore);
	//开启与pawn的重叠检测
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void AAureEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
	{
		Destroy();
	}
}


void AAureEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	Box->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxOverlap);
}

void AAureEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	bReached = true;

	//在设置的所有点位生成敌人
	for (AAureEnemySpawnPoint* SpawnPoint : SpawnPoints)
	{
		if (IsValid(SpawnPoint))
		{
			SpawnPoint->SpawnEnemy();
		}
	}

	//设置不在产生物理查询，直接销毁无法保存到存档
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



