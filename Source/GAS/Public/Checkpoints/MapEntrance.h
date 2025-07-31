// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoints/CheckPoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API AMapEntrance : public ACheckPoint
{
	GENERATED_BODY()

public:
	//构造函数
	explicit  AMapEntrance(const FObjectInitializer& ObjectInitializer);

	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	/* Highlight Interface */
	
	/* Save Interfacce */
	//从存档读取数据后，更新Actor
	virtual void LoadActor_Implementation() override;
	/*  End Save Interfacce*/

	//传送点需要传送到的目标地图
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	//传送到目标地图的出身点
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

protected:
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult) override;	
};
