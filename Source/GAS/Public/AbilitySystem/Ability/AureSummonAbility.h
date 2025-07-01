// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AureGameplayAbility.h"
#include "AureSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureSummonAbility : public UAureGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocation();

	UFUNCTION(BlueprintPure, Category="Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	//召唤数量
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	int32 NumMinions =5;

	//召唤物生成的角色类
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>>  MinionClasses;

	//召唤物距离召唤师最近的距离
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MinSpawnDistance = 50.f;
	//召唤物距离召唤师最远的距离
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MaxSpawnDistance = 250.f;

	//召唤物在召唤时的角度范围
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float SpawnSpread = 90.f;
};
