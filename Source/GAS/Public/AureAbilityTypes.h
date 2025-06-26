// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "AureAbilityTypes.generated.h"

/**
 * 
 */
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
