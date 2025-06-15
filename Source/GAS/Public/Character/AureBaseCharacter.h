// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AureBaseCharacter.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class GAS_API AAureBaseCharacter : public ACharacter,public IAbilitySystemInterface,public ICombatInterface 
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAureBaseCharacter();
	
	/** Combat Interface */

	
	/** end Combat Interface */
	
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> Weapon;


	UPROPERTY(EditAnywhere,Category = "AbilitySystem")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditAnywhere,Category = "AttributeSet")
	TObjectPtr<UAttributeSet> AttributeSet;

	/**
	 * 初始化能力演员信息
	 * 
	 * 此函数负责设置与能力相关的演员信息它可能包括为演员分配特定的能力、
	 * 设置能力的初始状态、或定义演员如何与这些能力交互这个函数可能在游戏或应用的
	 * 启动过程中被调用，或者在特定情况下，当演员需要重新初始化或恢复默认状态时
	 */
	virtual void InitAbilityActorInfo();

	//主属性
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes",DisplayName="主属性初始化")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	//次级属性
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes",DisplayName="次级属性初始化")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	//重要属性
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes",DisplayName="重要属性初始化")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;


	/**
	 * 对自己应用游戏效果
	 * 
	 * @param GameplayEffectClass 游戏效果的类，用于指定要应用的效果类型
	 * @param Level 效果的级别，可以影响效果的强度或持续时间
	 */
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	/**
	 * 初始化默认属性
	 * 
	 * 此函数用于在对象创建时设置默认的属性值它通常用于确保所有子类都具有一组标准的、初始化的属性
	 */
	virtual void InitializeDefaultAttributes() const;
public:	

	
	

};
