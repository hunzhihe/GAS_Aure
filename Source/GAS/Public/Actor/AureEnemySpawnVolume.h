// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureEnemySpawnPoint.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AureEnemySpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class GAS_API AAureEnemySpawnVolume : public AActor,public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	AAureEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* Save Interface End  */

	//当前怪物生成体积是否已经生成过敌人
	UPROPERTY(BlueprintReadOnly,SaveGame)
	bool bReached = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	//配置当前的生成体积所需生成的敌人
	UPROPERTY(EditAnywhere)
	TArray<AAureEnemySpawnPoint*> SpawnPoints;

private:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

};
