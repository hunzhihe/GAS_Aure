// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"

#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GAS.h"

AAureEnemy::AAureEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡

	AbilitySystemComponent  = CreateDefaultSubobject<UAureAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAureAttributeSet>(TEXT("AttributeSet"));


	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	
	// 设置角色模型的自定义深度模板值为红色
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	// 标记角色模型的渲染状态为脏，以确保在下一次渲染时更新
	GetMesh()->MarkRenderStateDirty();
	// 设置武器模型的自定义深度模板值为红色
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	// 标记武器模型的渲染状态为脏，以确保在下一次渲染时更新
	Weapon->MarkRenderStateDirty();
	
}

void AAureEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AAureEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAureEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

void AAureEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
}

void AAureEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	Cast<UAureAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}
