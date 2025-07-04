// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AureCharacter.h"

#include "AbilitySystemComponent.h"
#include "BlueprintAssetNodeSpawner.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "Game/AureGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AurePlayerController.h"
#include "Player/AurePlayerState.h"
#include "UI/HUD/AureHUD.h"


AAureCharacter::AAureCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true; //设置为true，角色将朝移动的方向旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f); //旋转速率
	GetCharacterMovement()->bConstrainToPlane = true; //约束到平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true; //设置了上面一项为true，且此项设置为true，则在开始时与地面对其

	//设置使用控制器选择Pitch，Yaw，Roll的值。
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//设置玩家职业
	CharacterClass = ECharacterClass::Elementalist;
	
}

void AAureCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//初始化ASC的ownerActor和AvatarActor
	InitAbilityActorInfo();

	//初始化角色技能
	AddCharacterAbilities();
}

void AAureCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

int32 AAureCharacter::GetPlayerLevel_Implementation()
{
	const AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->GetPlayerLevel();
}

void AAureCharacter::Die()
{
	Super::Die();
}


void AAureCharacter::InitAbilityActorInfo()
{
	 AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
     check(AurePlayerState);

	 //从PlayerState获取ASC和AS
	 AbilitySystemComponent = AurePlayerState->GetAbilitySystemComponent();
	 AttributeSet = AurePlayerState->GetAttributeSet();
	 //初始化ASC
	 AbilitySystemComponent->InitAbilityActorInfo(AurePlayerState, this);
	
	 //触发Actor的技能信息设置回调
	 Cast<UAureAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	//获取PC，再调用HUD
	 if (AAurePlayerController* AurePlayerController = Cast<AAurePlayerController>(GetController()))
	 {
		 if (AAureHUD* AureHUD =
		 	Cast<AAureHUD>(AurePlayerController->GetHUD()))
		 {
			 AureHUD->InitOverlay(
			 	AurePlayerController, AurePlayerState, AbilitySystemComponent, AttributeSet);
		 }
	 }

	//通过GE初始化角色主要属性
	  InitializeDefaultAttributes();
}
