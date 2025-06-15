// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AurePlayerState.h"

#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAurePlayerState::AAurePlayerState()
{
    // 创建并初始化能力系统组件
	AbilitySystemComponent = CreateDefaultSubobject<UAureAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    // 设置组件为需要复制到客户端
	AbilitySystemComponent->SetIsReplicated(true);
	// 设置游戏效果的复制模式为混合模式
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAureAttributeSet>(TEXT("AttributeSet"));

    // 设置网络更新频率为每秒100次
    SetNetUpdateFrequency(100.f);

}

void AAurePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	/*声明一些属性在游戏运行期间需要在网络中复制（Replication）,确保服务器和客户端之间的同步*/
	DOREPLIFETIME(AAurePlayerState, Level);
	DOREPLIFETIME(AAurePlayerState, XP);
	DOREPLIFETIME(AAurePlayerState, AttributePoints);
	DOREPLIFETIME(AAurePlayerState, SpellPoints);
	
}

UAbilitySystemComponent* AAurePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAurePlayerState::OnRep_Level(int32 OldLevel)
{
	
}

void AAurePlayerState::OnRep_XP(int32 OldXP)
{
}

void AAurePlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
}

void AAurePlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
}
