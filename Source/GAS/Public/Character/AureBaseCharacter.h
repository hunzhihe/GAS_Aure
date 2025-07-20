// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "AureBaseCharacter.generated.h"

class UNiagaraSystem;
class UGameplayAbility;
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
	virtual void Tick(float DeltaTime) override;
	/**
	 * 覆写 应用伤害给自身
	 * @see https://www.unrealengine.com/blog/damage-in-ue4
	 * @param DamageAmount		要施加的伤害数值
	 * @param DamageEvent		描述伤害细节的结构体，支持不同类型的伤害，如普通伤害、点伤害（FPointDamageEvent）、范围伤害（FRadialDamageEvent）等。
	 * @param EventInstigator	负责造成伤害的 Controller，通常是玩家或 AI 的控制器。
	 * @param DamageCauser		直接造成伤害的 Actor，例如爆炸物、子弹或掉落的石头。
	 * @return					返回实际应用的伤害值。这允许目标修改或减少伤害，然后将最终的值返回。
	 */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	
	/** Combat Interface */
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void IncrementMinionCount_Implementation(const int32 Amount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	virtual FOnDeath& GetOnDeathDelegate() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	virtual bool IsBeingShocked_Implementation() const override;
	virtual FOnDamageSignature& GetOnDamageDelegate() override;

	/** end Combat Interface */

	//ASC注册成功委托
	FOnASCRegistered OnASCRegistered;
	//角色死亡后触发的死亡委托
	FOnDeath OnDeath; 
    
	//传入伤害后得到结果后的委托
	FOnDamageSignature OnDamageDelegate;
	
    // 参数NetMulticast表明，该函数将在服务器执行，然后复制到每个客户端，传输属性Reliable表明该函数应该以可靠方式发送数据
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);
	

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	

	

	//当前角色是否处于眩晕状态
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	//当前角色是否处于燃烧状态
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;

	//当前角色是否处于被持续攻击状态（比如被闪电链攻击）
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeingShocked = false;

	UFUNCTION()
	virtual void OnRep_Stunned();

	UFUNCTION()
	virtual void OnRep_Burned();


	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> Weapon;


	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;



    //召唤的仆从数量
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	int32 MinionsCount = 0; 
	
	
	bool bDead = false;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

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

	void AddCharacterAbilities();

	/*角色死亡后溶解效果*/
	// 触发溶解效果的函数
	void Dissolve();
	
	// 启动角色溶解时间轴的函数
	// 参数: DynamicMaterialInstance - 动态材质实例，用于应用溶解效果
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	// 启动武器溶解时间轴的函数
	// 参数: DynamicMaterialInstance - 动态材质实例，用于应用溶解效果
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	// 角色溶解材质实例，可以在编辑器中设置，并在蓝图中读取
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	// 武器溶解材质实例，可以在编辑器中设置，并在蓝图中读取
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	/*角色死亡后溶解效果*/

	//火焰负面效果表现组件
	UPROPERTY(VisibleAnywhere) 
	TObjectPtr<UDebuffNiagaraComponent> BurnDeBuffComponent;

	//眩晕负面效果表现组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDebuffNiagaraComponent> StunDeBuffComponent;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;


	//注册用于监听负面标签变动的函数
	void DeBuffRegisterChanged();
	//眩晕标签变动后的回调
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	//当前角色的最大移动速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 600.f;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;
	
	//被动技能挂载的组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

};
