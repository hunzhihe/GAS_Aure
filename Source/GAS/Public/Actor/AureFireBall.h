// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AureProjectile.h"
#include "AureFireBall.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API AAureFireBall : public AAureProjectile
{
	GENERATED_BODY()

public:

	//执行蓝图时间轴事件，需要在蓝图中实现此事件
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();


	//当前火球返回的目标角色，默认是技能的释放者，在创建火球是创建
	UPROPERTY(BlueprintReadOnly) 
	TObjectPtr<AActor> ReturnToActor;

	virtual void OnHit() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:

	virtual void BeginPlay() override;


};
