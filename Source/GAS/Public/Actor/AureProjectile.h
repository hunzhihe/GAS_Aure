// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "AureProjectile.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class GAS_API AAureProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AAureProjectile();

    //投射物移动组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	//放置伤害GE
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

protected:
	
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	//命中特效
	virtual void OnHit();

	//销毁
	virtual void Destroyed() override;


	// 碰撞体重叠事件
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	//碰撞体
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	bool IsValidOverlap(AActor* OtherActor);
	
	bool bHit = false;

	//存储循环音效的变量，后续用于删除
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

private:

	
    
	
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	// （如碰撞或爆炸）发生时播放的粒子效果
	UPROPERTY(EditAnywhere)
		TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	// 一次性声音效果，用于增强环境反馈
	UPROPERTY(EditAnywhere)
		TObjectPtr<USoundBase> ImpactSound;
	
	// 循环播放的声音效果，可能用于背景音或持续的机械声等
	UPROPERTY(EditAnywhere)
		TObjectPtr<USoundBase> LoopingSound;
};

