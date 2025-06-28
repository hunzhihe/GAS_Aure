// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UDebuffNiagaraComponent();

	//用来标识粒子系统的标签
    UPROPERTY(EditDefaultsOnly)
	FGameplayTag DebuffTag;

protected:
	virtual void BeginPlay() override;

	//当前的负面标签变动回调
	void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	//在角色死亡时的回调
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
