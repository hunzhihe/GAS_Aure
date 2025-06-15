// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureBaseCharacter.h"
#include "Interaction/HighlightInterface.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API AAureEnemy : public AAureBaseCharacter,public IHighlightInterface
{
	GENERATED_BODY()
public:
	AAureEnemy();

	/*IHighlightInterface*/
	    // 实现高亮显示Actor的功能
	virtual void HighlightActor_Implementation() override;
	    // 实现取消高亮显示Actor的功能
	virtual void UnHighlightActor_Implementation() override;
	
	/*IHighlightInterface 结束*/


	
	/** Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	/** end Combat Interface */

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
};
