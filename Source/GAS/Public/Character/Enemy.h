// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AureBaseCharacter.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Enemy.generated.h"

class UWidgetComponent;
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
	virtual void Die() override;
	/** end Combat Interface */

	//受击标签数量改变时的回调
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// 是否被击
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	// 移动速度
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;

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
};
