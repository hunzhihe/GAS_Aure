// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/AureBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

struct FAureGameplayTags;
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

FVector AAureBaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FAureGameplayTags& GameplayTags = FAureGameplayTags::Get();
	// 判断是否匹配武器插槽
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	// 判断是否匹配左手插槽
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	// 判断是否匹配右手插槽
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	// 匹配尾部插槽
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

UAnimMontage* AAureBaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAureBaseCharacter::Die()
{
	//当角色死亡时，将武器从角色身上移除
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

bool AAureBaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAureBaseCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAureBaseCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAureBaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAureBaseCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	// 遍历攻击动画蒙太奇数组，以找到匹配指定标签的蒙太奇
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
	    // 检查当前蒙太奇的标签是否与指定的蒙太奇标签精确匹配
	    if (TaggedMontage.MontageTag.MatchesTagExact(MontageTag))
	    {
	        // 如果找到匹配的蒙太奇，立即返回它
	        return TaggedMontage;
	    }
	}
	// 如果没有找到匹配的蒙太奇，返回一个默认的FTaggedMontage对象
	return FTaggedMontage();
}

int32 AAureBaseCharacter::GetMinionCount_Implementation()
{
	return  MinionsCount;
}

void AAureBaseCharacter::IncrementMinionCount_Implementation(const int32 Amount)
{
	MinionsCount += Amount;
}

ECharacterClass AAureBaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}


void AAureBaseCharacter::MulticastHandleDeath_Implementation()
{
	//开启武器物理效果
	Weapon->SetSimulatePhysics(true);
	//重力模拟
	Weapon->SetEnableGravity( true);
	//物理碰撞通道
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);



	//开启角色物理效果
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity( true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//添加静态碰撞通道
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);

	//关闭角色碰撞体碰撞通道，避免其对武器和角色模拟物理效果产生影响
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);

	//角色死亡时，调用Dissolve方法
	Dissolve();
	

	//设置死亡状态
	bDead = true;
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

void AAureBaseCharacter::AddCharacterAbilities()
{
	// 获取当前对象的技能系统组件，并将其转换为UAureAbilitySystemComponent类型
	UAureAbilitySystemComponent* AureAbilitySystemComponent = CastChecked<UAureAbilitySystemComponent>(
		GetAbilitySystemComponent());
	
	// 检查当前对象是否具有权限执行此操作
	if (!HasAuthority())
	{
	    return;
	}
	
	// 为角色添加初始技能
	AureAbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
	AureAbilitySystemComponent->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAureBaseCharacter::Dissolve()
{
	// 检查溶解材质实例是否有效
	if (IsValid(DissolveMaterialInstance))
	{
	    // 创建动态材质实例
	    UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
	    // 为网格设置材质
	    GetMesh()->SetMaterial(0, DynamicMatInst);
	    // 启动溶解时间轴
	    StartDissolveTimeline(DynamicMatInst);
	}
	
	// 检查武器溶解材质实例是否有效
	if (IsValid(WeaponDissolveMaterialInstance))
	{
	    // 创建动态材质实例
	    UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
	    // 为武器设置材质
	    Weapon->SetMaterial(0, DynamicMatInst);
	    // 启动武器溶解时间轴
	    StartWeaponDissolveTimeline(DynamicMatInst);
	}
}







