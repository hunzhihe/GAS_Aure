// Fill out your copyright notice in the Description page of Project Settings.


#include "AureAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AureGameplayTags.h"

UAureAssetManager& UAureAssetManager::Get()
{
	check(GEngine);

	UAureAssetManager* AureAssetManager = Cast<UAureAssetManager>(GEngine->AssetManager);
	return *AureAssetManager;
}

void UAureAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAureGameplayTags::InitializeNativeGameplayTags();

	//获取目标数据的必须项
	UAbilitySystemGlobals::Get().InitGlobalData();
}
