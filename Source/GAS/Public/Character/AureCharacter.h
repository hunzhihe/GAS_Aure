// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureBaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "AureCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API AAureCharacter : public AAureBaseCharacter,public IPlayerInterface 
{
	GENERATED_BODY()

public:
	AAureCharacter();
	
	// 服务端初始化ASC
	virtual void PossessedBy(AController* NewController) override;
	// 客户端初始化ASC
	virtual void OnRep_PlayerState() override;

	/* ICombatInterface战斗接口 */
	virtual int32 GetPlayerLevel_Implementation() override;
	/* ICombatInterface战斗接口 结束 */

protected:
	virtual  void InitAbilityActorInfo() override;
	
};
