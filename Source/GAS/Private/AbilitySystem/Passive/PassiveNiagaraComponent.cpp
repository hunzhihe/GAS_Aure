// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AureGameplayTags.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UAureAbilitySystemComponent* AureASC = Cast<UAureAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AureASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		//初始化时，读取存档已经结束，技能也设置完成，所以需要读取存档并更新显示
		if (AureASC->GetStatusFromAbilityTag(PassiveSpellTag) == FAureGameplayTags::Get().Abilities_Status_Equipped)
		{
			OnPassiveActivate(PassiveSpellTag, true);
		}
	}
	else if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		//初始化还没完成时
		//AddWeakLambda 这种绑定方式的主要好处是，当绑定的对象被销毁时，委托不会保持对象的引用，从而避免悬空指针问题和内存泄漏。
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this,[this](UAbilitySystemComponent* InASC)
		{
			if(UAureAbilitySystemComponent* AureASC = Cast<UAureAbilitySystemComponent>(InASC))
			{
				AureASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				if (AureASC->GetStatusFromAbilityTag(PassiveSpellTag) == FAureGameplayTags::Get().Abilities_Status_Equipped)
				{
					OnPassiveActivate(PassiveSpellTag, true);
				}
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	//判断技能标签是否一致
	if(AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		//判断是否需要激活
		if(bActivate)
		{
			//不需要重复激活
			if(!IsActive()) Activate();
		}
		else
		{
			Deactivate();
		}
	}
}
