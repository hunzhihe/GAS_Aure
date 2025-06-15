// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AureAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UAureAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:

	static UAureAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
};
