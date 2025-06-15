// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/AureBaseCharacter.h"

#include "AbilitySystemComponent.h"

// Sets default values
AAureBaseCharacter::AAureBaseCharacter()
{
 	
	// 禁用Actor的Tick功能，表示该Actor不需要每帧更新
	PrimaryActorTick.bCanEverTick = true;
	
	// 创建一个默认的USkeletalMeshComponent子对象，命名为"Weapon"
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	// 将武器网格模型附加到角色网格模型的"WeaponHandSocket"插槽上
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	// 设置武器的碰撞模式为"NoCollision"
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

}



UAbilitySystemComponent* AAureBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAureBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAureBaseCharacter::InitAbilityActorInfo()
{
}

void AAureBaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	// 检查能力系统组件的有效性
	check(IsValid(GetAbilitySystemComponent()));
	// 检查游戏效果类是否已指定
	check(GameplayEffectClass);
	
	// 创建一个游戏效果上下文句柄
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	// 将效果的来源对象添加到上下文句柄中
	ContextHandle.AddSourceObject(this);
	
	// 创建并配置一个游戏效果规范句柄
	const FGameplayEffectSpecHandle SpecHandle =
	    GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	
	// 应用配置好的游戏效果规范到目标能力系统组件
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
	    *SpecHandle.Data.Get(), GetAbilitySystemComponent());
	
}

void AAureBaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.0f);
}







