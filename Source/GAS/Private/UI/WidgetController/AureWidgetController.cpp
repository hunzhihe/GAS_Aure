// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AureWidgetController.h"

#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "Player/AurePlayerController.h"
#include "Player/AurePlayerState.h"

void UAureWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAureWidgetController::BroadcastInitialValues()
{
}

void UAureWidgetController::BindCallbacksToDependencies()
{
}

AAurePlayerController* UAureWidgetController::GetAurePC()
{
	if (AurePlayerController == nullptr)
	{
		AurePlayerController = Cast<AAurePlayerController>(PlayerController);
	}
	return AurePlayerController;
}

AAurePlayerState* UAureWidgetController::GetAurePS()
{
	if (AurePlayerState == nullptr)
	{
		AurePlayerState = Cast<AAurePlayerState>(PlayerState);
	}
	return AurePlayerState;
}

UAureAbilitySystemComponent* UAureWidgetController::GetAureASC()
{
	if (AureAbilitySystemComponent == nullptr)
	{
		AureAbilitySystemComponent = Cast<UAureAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AureAbilitySystemComponent;
}

UAureAttributeSet* UAureWidgetController::GetAureAS()
{
	if (AureAttributeSet == nullptr)
	{
		AureAttributeSet = Cast<UAureAttributeSet>(AttributeSet);
	}
	return AureAttributeSet;
}
