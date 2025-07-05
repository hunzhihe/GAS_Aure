// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AureAttributeSet.generated.h"

/*定义属性访问器宏, 生成相对的函数，当前设置会生成四个函数，获取属性，获取值，设置值，初始化值*/
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//DECLARE_DELEGATE_RetVal(FGameplayAttribute,FGameplayAttributeSignature)
//使用上面的委托，在BindStatic时，查看源码，实际上就是绑定到函数指针上了，因此我们这里直接把Map映射从委托改成AS中的获取属性函数指针，
 template<class T>
 using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;


// 定义一个结构体FEffectProperties，用于存储效果的属性信息
USTRUCT()
struct FEffectProperties
{
    GENERATED_BODY()

    // 默认构造函数
    FEffectProperties(){}

    // 存储游戏玩法效果的上下文句柄
    FGameplayEffectContextHandle EffectContextHandle;

    // UPROPERTY宏用于自动生成属性的访问函数和序列化代码
    // 源能力系统组件指针
    UPROPERTY()
    UAbilitySystemComponent* SourceASC = nullptr;

    // 源角色演员指针
    UPROPERTY()
    AActor* SourceAvatarActor = nullptr;

    // 源控制器指针
    UPROPERTY()
    AController* SourceController = nullptr;

    // 源角色指针
    UPROPERTY()
    ACharacter* SourceCharacter = nullptr;

    // 目标能力系统组件指针
    UPROPERTY()
    UAbilitySystemComponent* TargetASC = nullptr;

    // 目标角色演员指针
    UPROPERTY()
    AActor* TargetAvatarActor = nullptr;

    // 目标控制器指针
    UPROPERTY()
    AController* TargetController = nullptr;

    // 目标角色指针
    UPROPERTY()
    ACharacter* TargetCharacter = nullptr;
};

/**
 * 
 */
UCLASS()
class GAS_API UAureAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAureAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	 * 预属性改变，在属性值改变之前调用
	 * @param Attribute 属性
	 * @param NewValue 新值
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// 属性值改变后
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/*
	 * GE激活，属性值改变之后调用
	 * @param Data 属性改变数据
	 * 主要是去修正数值，在PreAttributeChange回调里面，只能修正显示的结果，实际结果没有变
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


	//将标签映射到属性的静态函数函数
    TMap<FGameplayTag, FGameplayAttribute(*)()> TagsToAttributes;


	
	/*
	 * Primary Attributes 主属性
	 */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;//力量
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;//智力
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Intelligence);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;//韧性
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Resilience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;//体力
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Vigor);


	/*
	 * Secondary Attributes 次要属性 受主属性影响
	 */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;//护甲
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;//护甲穿透
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;//格挡率
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, BlockChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;//暴击率
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, CriticalHitChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;//暴击伤害
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;//暴击抗性
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, CriticalHitResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;//生命值回复
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, HealthRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;//法力回复
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, ManaRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;//最大生命值
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;//最大法力
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, MaxMana);

	/*
	 * Resistance Attributes  抗性
	 */

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Resistance Attributes")
	FGameplayAttributeData FireResistance;//火焰抗性
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, FireResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Resistance Attributes")
	FGameplayAttributeData LightningResistance;//闪电抗性
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, LightningResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category = "Resistance Attributes")
	FGameplayAttributeData ArcaneResistance;//法术抗性
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, ArcaneResistance);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Resistance Attributes")
	FGameplayAttributeData PhysicalResistance;//物理抗性
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, PhysicalResistance);


	/*
	 * Vital Attributes  重要属性
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, Mana);


	/*
	 * Meta Attributes  元属性
	 */

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UAureAttributeSet, IncomingXP);
	

	// 当Health属性值在网络中同步更新时调用
	/**
	 * 当Health属性值发生变化时，服务器通知客户端的新值
	 * 使用GAMEPLAYATTRIBUTE_REPNOTIFY宏来处理属性的复制通知
	 * @param OldHealth 更改前的Health值，用于比较和处理变化
	 */
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;	

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;

private:

	//处理传入的参数为伤害属性时，处理的逻辑
	void HandleInComingDamage(const FEffectProperties& Props);

	//处理当前伤害触发负面效果，处理的逻辑
	void HandleDebuff(const FEffectProperties& Props);

	//处理传入的参数为经验值时，处理的逻辑
	void HandleIncomingXP(const FEffectProperties& Props);
	
	// 设置效果属性 生成结构体的值
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	// 显示伤害浮点数
	static void ShowFloatingText(const FEffectProperties& Props, const float Damage, bool IsBlockedHit, bool IsCriticalHit);
    // 发送XP事件
	void SendXPEvent(const FEffectProperties& Props);

	bool bTopOffHealth = false;
	bool bTopOffMana = false;
};
