// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AureFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Interaction/CombatInterface.h"


void AAureFireBall::OnHit()
{
	if(GetOwner())
	{
		//设置GameplayCue播放位置
		FGameplayCueParameters Parameters;
		Parameters.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAureGameplayTags::Get().GameplayCue_FireBlast, Parameters);
	}
	//将音乐停止后会自动销毁
	if(LoopingSoundComponent)
	{
		//循环组件暂停并销毁
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	
	bHit = true;
	
}

void AAureFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(GetInstigator() == OtherActor) return; //判断和碰撞体接触的目标是否为自身

	//如果不是敌人，将不执行后续
	if(!UAuraAbilitySystemLibrary::IsNotFriend(GetInstigator(), OtherActor)) return;

	//目标未继承战斗接口，返回
	if(!OtherActor->Implements<UCombatInterface>()) return;
	
	//播放击中特效 当前服务器端未调用销毁时，直接播放
	if(!bHit) OnHit();
	
	//在重叠后，销毁自身
	if(HasAuthority()) //是否为服务器，如果服务器端，将执行应用GE，并通过GE的自动同步到客户端
	{
		//为目标应用GE
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			//死亡冲击的力度和方向
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			
			//通过配置项应用给目标ASC
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AAureFireBall::BeginPlay()
{
	Super::BeginPlay();

	//调用开始时间线修改
	StartOutgoingTimeline();
}
