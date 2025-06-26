// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureBaseCharacter.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Enemy.generated.h"

class UBehaviorTree;
class AAureAIController;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class GAS_API AAureEnemy : public AAureBaseCharacter,public IHighlightInterface,public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAureEnemy();
	
	//在Pawn被控制器（PlayerController和AIController都行）控制时该函数触发回调
	virtual void PossessedBy(AController* NewController) override;

	/*IHighlightInterface*/
	    // 实现高亮显示Actor的功能
	virtual void HighlightActor_Implementation() override;
	    // 实现取消高亮显示Actor的功能
	virtual void UnHighlightActor_Implementation() override;
	
	/*IHighlightInterface 结束*/
	
	
	/** Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die() override;
	/** end Combat Interface */


    /*IEnemyInterface*/

	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;

	/*end IEnemyInterface*/
	

	//受击标签数量改变时的回调
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// 是否被击
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	// 移动速度
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;


	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	//死亡后角色存在时间
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	//血条控件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UWidgetComponent> HealthBar;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	//行为树
	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	//保存保存转换为RPGAIController的实例
	UPROPERTY()
	TObjectPtr<AAureAIController> AureAIController;
};
