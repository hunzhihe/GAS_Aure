// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
	
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	//获取战斗接口
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	
	//获取角色身上的ASC
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		//监听负面标签变动
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		
	}
	//如果绑定时，ASC未初始化成功，则监听ASC创建完成委托，完成对负面标签的监听
	else if (CombatInterface)
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this,[this](UAbilitySystemComponent* InASC)
		{
			//监听负面标签变动回调
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
		});
	}
}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount > 0)
	{
		Activate(); //绑定的负面标签大于0，激活特效
	}
	else
	{
		Deactivate(); //没有对应标签，关闭激活
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
