// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "AureAbilityTypes.generated.h"

class UGameplayEffect;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}
	
	
	// 属性：世界上下文对象，用于获取世界信息
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
	
	// 属性：伤害游戏效果类，定义了伤害的Gameplay效果
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	
	// 属性：源能力系统组件，表示伤害的来源
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;
	
	// 属性：目标能力系统组件，表示伤害的目标
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	//技能造成的多种伤害和伤害类型
	UPROPERTY(BlueprintReadWrite)
	TMap<FGameplayTag, float> DamageTypes;
	
	// 属性：技能等级，用于计算伤害
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	
	// 属性：负面伤害类型标签，用于区分不同的负面伤害类型
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DebuffDamageType = FGameplayTag();
	
	// 属性：负面效果触发概率
	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;
	
	// 属性：负面效果的伤害值
	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;
	
	// 属性：负面效果的持续时间
	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
	
	// 属性：负面效果的频率
	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;
	
	// 属性：死亡冲击的强度
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	
	// 属性：死亡冲击的方向和大小
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;
	
	// 属性：击退力的强度
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;
	
	// 属性：击退效果触发概率
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
	
	// 属性：击退力的方向和大小
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;
	
	// 属性：是否为径向伤害
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;
	
	// 属性：径向伤害的内半径
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	
	// 属性：径向伤害的外半径
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	
	// 属性：径向伤害的中心点
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
	
};




USTRUCT(BlueprintType)
struct FAureGameplayEffectContext: public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	//格挡的获取和设置函数
	bool IsBlockHit() const {return bIsBlockedHit;}
	void SetBlockHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit;}

	//暴击的获取和设置函数
	bool IsCriticalHit() const { return bIsCriticalHit;}
	void SetCriticalHit(bool bInIsCriticalHit) {bIsCriticalHit = bInIsCriticalHit;}

	//成功应用负面效果的获取和设置函数
	bool IsSuccessfulDeBuff() const { return bIsSuccessfulDeBuff;}
	void SetSuccessfulDeBuff(bool bInIsSuccessfulDeBuff) {bIsSuccessfulDeBuff = bInIsSuccessfulDeBuff;}

	//获取和设置负面效果的伤害
	float GetDeBuffDamage() const { return DeBuffDamage;}
	void SetDeBuffDamage(float InDeBuffDamage) {DeBuffDamage = InDeBuffDamage;}
	
	//获取和设置负面效果的持续时间
	float GetDeBuffDuration() const { return DeBuffDuration;}
	void SetDeBuffDuration(float InDeBuffDuration) {DeBuffDuration = InDeBuffDuration;}
	
	//获取和设置负面效果的触发频率间隔
	float GetDeBuffFrequency() const { return DeBuffFrequency;}
	void SetDeBuffFrequency(float InDeBuffFrequency) {DeBuffFrequency = InDeBuffFrequency;}
	
	//获取和设置负面效果的伤害类型
	TSharedPtr<FGameplayTag> GetDebuffDamageType() const { return DamageType;}
	void SetDebuffDamageType(const TSharedPtr<FGameplayTag>& InDamageType) {DamageType = InDamageType;}

	//获取和设置死亡冲击的强度
	FVector GetDeathImpulse() const { return DeathImpulse;}
	void SetDeathImpulse(const FVector& InDeathImpulse) {DeathImpulse = InDeathImpulse;}

	//获取和设置击退力量
	FVector GetKnockbackForce() const { return KnockbackForce;}
	void SetKnockbackForce(const FVector& InKnockbackForce) {KnockbackForce = InKnockbackForce;}

	//获取和设置是否为径向伤害
	bool IsRadialDamage() const { return bIsRadialDamage;}
	void SetRadialDamage(bool bInIsRadialDamage) {bIsRadialDamage = bInIsRadialDamage;}

	//获取和设置径向伤害的内半径
	float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius;}
	void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius) {RadialDamageInnerRadius = InRadialDamageInnerRadius;}

	//获取和设置径向伤害的外半径
	float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius;}
	void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius) {RadialDamageOuterRadius = InRadialDamageOuterRadius;}

	//获取和设置径向伤害的源点
	FVector GetRadialDamageOrigin() const { return RadialDamageOrigin;}
	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin) {RadialDamageOrigin = InRadialDamageOrigin;}
	

	
	//返回用于序列化的结构体
	virtual UScriptStruct *GetScriptStruct() const override {return StaticStruct();}

	//序列化,参数分别为：用于序列化和反序列化的archive，查找或记录对象间的引用关系的包映射，输出是否序列化成功与否
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	//复制,主要是对Hit Result进行深拷贝
	virtual FAureGameplayEffectContext* Duplicate() const override
	{
		FAureGameplayEffectContext* NewContext = new FAureGameplayEffectContext();
		*NewContext = *this; //WithCopy 设置为true，就可以通过赋值操作进行拷贝
		if (GetHitResult())
		{
			// 深拷贝Hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
protected:
	
	//是否格挡
	UPROPERTY()
	bool bIsBlockedHit = false; 

	//是否暴击
	UPROPERTY()
	bool bIsCriticalHit = false;
	
	//成功应用负面效果
	UPROPERTY()
	bool bIsSuccessfulDeBuff = false;
	
	//负面效果每次造成的伤害
	UPROPERTY()
	float DeBuffDamage = 0.f;
	
	//负面效果持续时间
	UPROPERTY()
	float DeBuffDuration = 0.f;
	
	//负面效果触发频率间隔
	UPROPERTY()
	float DeBuffFrequency = 0.f;
	

	//负面效果的伤害类型
	TSharedPtr<FGameplayTag> DamageType;

	
	// 当角色死亡时应用的冲量，用于模拟死亡时的物理效果
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	// 击退力量的方向和强度，用于在受到攻击时将角色推开
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;
	
	// 是否为径向（圆形）损伤，用于确定损伤类型
	UPROPERTY()
	bool bIsRadialDamage = false;
	
	// 径向损伤的内半径，在此范围内的目标将受到全额损伤
	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;
	
	// 径向损伤的外半径，介于内半径和外半径之间的目标将受到渐变损伤
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;
	
	// 径向损伤的起源点，损伤从此点向外扩展
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
	
};

template<>
struct TStructOpsTypeTraits< FAureGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAureGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
