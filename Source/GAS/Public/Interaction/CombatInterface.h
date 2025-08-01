// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"


class UNiagaraSystem;
//Actor初始化ASC完成后委托
DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCRegistered, UAbilitySystemComponent*)
//角色死亡委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);
//返回范围伤害能够对自身造成的伤害，在TakeDamage里广播
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageSignature, float /*范围伤害造成的最终数值*/); 

// 定义一个结构体FTaggedMontage，用于在蓝图中使用
USTRUCT(BlueprintType)
struct FTaggedMontage
{
    // 自动生成结构体所需的代码
    GENERATED_BODY()

    // 定义一个动画蒙太奇资源，只读，用于编辑默认设置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* Montage = nullptr;

    // 定义动画蒙太奇的游戏玩法标签，只读，用于编辑默认设置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag MontageTag;

    // 定义动画蒙太奇中插槽的游戏玩法标签，只读，用于编辑默认设置
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag SocketTag;

    // 定义影响声音资源，只读，用于编辑默认设置,攻击时的触发音效
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USoundBase* ImpactSound = nullptr;
};


struct FGameplayTag;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAS_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetPlayerLevel();


	//获取战斗socket位置
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);


	//蓝图事件，更新目标朝向
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	//获取受击动画
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();
	
	//获取当前角色是否死亡
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const; 

	//获取角色的Avatar
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	//获取攻击动画
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages();

	//获取角色的受伤特效
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect(); 

    //通过标签获取对应结构体
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);
	
	//死亡函数，子类重写
	virtual void Die(const FVector& DeathImpulse) = 0;

	//获取角色拥有的仆从数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount();

	//设置角色仆从的增长数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncrementMinionCount(const int32 Amount);

	//获取角色的职业信息
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass();

	//获取ASC注册成功后的委托
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() = 0;
	//获取死亡委托
	virtual FOnDeath& GetOnDeathDelegate() = 0;

	//设置是否持续施法
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetInShockLoop(bool bInLoop);

	/**
	 * 获取角色使用的武器指针
	 * @return 武器骨骼网格体组件
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetWeapon();


	/**
	 * 获取角色是否处于闪电链攻击状态
	 * @return 布尔值，如果处于返回true
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsBeingShocked() const;

	/**
	 * 设置角色是否处于闪电链攻击状态
	 * @param bInShock 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsBeingShocked(bool bInShock);

	/**
	 * 获取角色受到伤害触发的委托，由于委托是创建在角色基类里的，这里可以通过添加struct来实现前向声明，不需要在头部声明一遍。
	 * @return 委托
	 */
	virtual FOnDamageSignature& GetOnDamageDelegate() = 0; 


	
};
