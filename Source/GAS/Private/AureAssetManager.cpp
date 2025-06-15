// Fill out your copyright notice in the Description page of Project Settings.


#include "AureAssetManager.h"

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
}
