// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AureCharacter.h"

#include "AbilitySystemComponent.h"
#include "BlueprintAssetNodeSpawner.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "Game/AureGameModeBase.h"
#include "Game/LocalScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

	//设置相机
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;


	//设置升级特效组件
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent()); 
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	
}

void AAureCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//初始化ASC的ownerActor和AvatarActor
	InitAbilityActorInfo();


	//加载存档
	LoadProgress();

	if (AAureGameModeBase* AureGameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AureGameMode->LoadWorldState(GetWorld());
	}
	
	//初始化角色技能
	//AddCharacterAbilities();
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

void AAureCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
}

void AAureCharacter::AddToXP_Implementation(int32 InXP)
{
	AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	 check(AurePlayerState);
	 AurePlayerState->AddToXP(InXP);
}

void AAureCharacter::LevelUp_Implementation()
{
	//播放升级特效
	MulticastLevelUpParticles();
}

int32 AAureCharacter::GetXP_Implementation() const
{
	const AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->GetXP();
}

int32 AAureCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAureCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAureCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void AAureCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	AurePlayerState->AddToLevel(InPlayerLevel);

	if (UAureAbilitySystemComponent* AureASC = Cast<UAureAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AureASC->UpdateAbilityStatuses(AurePlayerState->GetPlayerLevel());
	}
}

void AAureCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	AurePlayerState->AddToAttributePoints(InAttributePoints);
}

void AAureCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	AurePlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAureCharacter::GetAttributePoints_Implementation() const
{
	AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->GetAttributePoints();
}

int32 AAureCharacter::GetSpellPoints_Implementation() const
{
	AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>();
	check(AurePlayerState);
	return AurePlayerState->GetSpellPoints();
}

void AAureCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if(AAurePlayerController* AurePlayerController = Cast<AAurePlayerController>(GetController()))
	{
		AurePlayerController->ShowMagicCircle(DecalMaterial);
		//隐藏鼠标
		AurePlayerController->bShowMouseCursor = false;
	}
	
}

void AAureCharacter::HideMagicCircle_Implementation() const
{
	if (AAurePlayerController* AurePlayerController = Cast<AAurePlayerController>(GetController()))
	{
		AurePlayerController->HideMagicCircle();
		//显示鼠标
		 AurePlayerController->bShowMouseCursor = true;
	}
}

void AAureCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if(const AAureGameModeBase* GameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//获取存档
		ULocalScreenSaveGame* SaveGameData = GameMode->RetrieveInGameSaveData();
		if(SaveGameData == nullptr) return;
		
		//修改存档数据
		SaveGameData->PlayerStartTag = CheckpointTag;
		//将检查点添加到已激活数组，（并去重）
		SaveGameData->ActivatedPlayerStartTags.AddUnique(CheckpointTag);

		//修改玩家相关
		if (const AAurePlayerState* AurePlayerState = GetPlayerState<AAurePlayerState>())
		{
			SaveGameData->XP = AurePlayerState->GetXP();
			SaveGameData->PlayerLevel = AurePlayerState->GetPlayerLevel();
			SaveGameData->AttributePoints = AurePlayerState->GetAttributePoints();
			SaveGameData->SpellPoints = AurePlayerState->GetSpellPoints();
		}

		//修改主要属性
		SaveGameData->Strength = UAureAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Intelligence = UAureAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Vigor = UAureAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Resilience = UAureAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());


        SaveGameData->bFirstTimeLoadIn = false;

		if (!HasAuthority()) return;
		

		UAureAbilitySystemComponent* AureASC = Cast<UAureAbilitySystemComponent>(GetAbilitySystemComponent());
		//将存档内的技能数组清空
		SaveGameData->SavedAbilities.Empty();

		//使用ASC里创建的ForEach函数循环获取角色的技能，并生成技能结构体保存
		FForEachAbility SaveAbilityDelegate;
		SaveAbilityDelegate.BindLambda([this,AureASC,SaveGameData](const FGameplayAbilitySpec& AbilitySpec)
		{
			//获取技能标签和技能数据
			FGameplayTag AbilityTag = UAureAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo( this);
			FAureAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			//创建技能结构体
			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilityType = Info.AbilityType;
			SavedAbility.AbilityInputTag = AureASC->GetInputTagFromSpec(AbilitySpec);
			SavedAbility.AbilityStatus = AureASC->GetStatusFromSpec(AbilitySpec);
			SavedAbility.AbilityLevel = AbilitySpec.Level;


			SaveGameData->SavedAbilities.AddUnique(SavedAbility);
		});

		//调用ForEach技能来执行存储到存档
		AureASC->ForEachAbility(SaveAbilityDelegate);
		
		//保存存档
		GameMode->SaveInGameProgressData(SaveGameData);
	}
	
}

TSubclassOf<UGameplayEffect> AAureCharacter::GetSecondaryAttributes_Implementation()
{
	return DefaultSecondaryAttributes;
}

TSubclassOf<UGameplayEffect> AAureCharacter::GetVitalAttributes_Implementation()
{
	return DefaultVitalAttributes;
}


void AAureCharacter::LoadProgress() 
{
	if(const AAureGameModeBase* GameMode = Cast<AAureGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//获取存档
		ULocalScreenSaveGame* SaveGameData = GameMode->RetrieveInGameSaveData();
		if(SaveGameData == nullptr) return;

		//修改玩家相关
		if(AAurePlayerState* AurePlayerState = Cast<AAurePlayerState>(GetPlayerState()))
		{
			AurePlayerState->SetLevel(SaveGameData->PlayerLevel,false);
			AurePlayerState->SetXP(SaveGameData->XP);
			AurePlayerState->SetAttributePoints(SaveGameData->AttributePoints);
			AurePlayerState->SetSpellPoints(SaveGameData->SpellPoints);
		}

		//判断是否为第一次加载存档，如果第一次，属性没有相关内容
		if(SaveGameData->bFirstTimeLoadIn)
		{
			//如果第一次加载存档，使用默认GE初始化主要属性
			InitializeDefaultAttributes();

			//初始化角色技能
			AddCharacterAbilities();
		}
		else
		{
			//如果不是第一次，将通过函数库函数通过存档数据初始化角色属性
			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveGameData);

			//初始化角色技能
			if (UAureAbilitySystemComponent* AureASC = Cast<UAureAbilitySystemComponent>(AbilitySystemComponent))
			{
				AureASC->AddCharacterAbilitiesFromSaveData(SaveGameData);
			}

			//从存档中读取关卡状态
			GameMode->LoadWorldState(GetWorld());
		}
	}
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

	//注册监听负面标签变动
	DeBuffRegisterChanged();

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
	 // InitializeDefaultAttributes();

	//调用ASC广播
	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

void AAureCharacter::MulticastLevelUpParticles_Implementation() const
{
	// 检查LevelUpNiagaraComponent是否有效，确保组件已经初始化并且没有被销毁
    if(IsValid(LevelUpNiagaraComponent))
    {
    // 获取摄像机的当前位置，用于计算粒子系统的旋转方向
    const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
    // 获取粒子系统的当前位置，用于计算其相对于摄像机的旋转
    const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
    // 计算摄像机到粒子系统的方向，并将其转换为旋转
    const FRotator TopCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();
    // 设置粒子系统的旋转，使其面向摄像机
    LevelUpNiagaraComponent->SetWorldRotation(TopCameraRotation);
    // 激活粒子系统，使其开始播放效果
    LevelUpNiagaraComponent->Activate(true);
    }
}
