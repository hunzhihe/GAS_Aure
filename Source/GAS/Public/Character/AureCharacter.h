// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureBaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "AureCharacter.generated.h"

class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
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
	virtual void Die() override;
	/* ICombatInterface战斗接口 结束 */

	/* IPlayerInterface玩家接口 */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	/* IPlayerInterface玩家接口 结束 */

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer;

	// 等级提升特效
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent; 

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	virtual  void InitAbilityActorInfo() override;


	//在多人游戏中，让每个客户端上播放升级特效
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
	
};
