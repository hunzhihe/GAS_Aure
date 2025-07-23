// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"

#include "AureGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
#include "AI/AureAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GAS.h"
#include "UI/Widget/AureUserWidget.h"

AAureEnemy::AAureEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡

	AbilitySystemComponent  = CreateDefaultSubobject<UAureAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAureAttributeSet>(TEXT("AttributeSet"));

	// 创建生命值条，并将其绑定到根节点
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	BaseWalkSpeed =250;
	

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

void AAureEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}
	// 将控制器转换为AAureAIController类型
	AureAIController = Cast<AAureAIController>(NewController);
	
	// 初始化AI控制器的黑板组件
	AureAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	
	// 运行行为树
	AureAIController->RunBehaviorTree(BehaviorTree);

	//设置黑板键的值
	AureAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AureAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
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

void AAureEnemy::Die(const FVector& DeathImpulse)
{              
    
	if (AureAIController)
	{
		AureAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}
	SetLifeSpan(LifeSpan);
	
	Super::Die(DeathImpulse);
}

AActor* AAureEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAureEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

void AAureEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	if (AureAIController && AureAIController->GetBlackboardComponent())
	{
		//设置黑板键的值
		AureAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AAureEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	if (AureAIController && AureAIController->GetBlackboardComponent())
	{
		//设置黑板键的值
		AureAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stuned"), bIsStunned);
	}
}

void AAureEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	//给敌人添加起始技能
	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	

	InitAbilityActorInfo();

	if (UAureUserWidget* Widget = Cast<UAureUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		Widget->SetWidgetController( this);
	}
	if(const UAureAttributeSet* AS = Cast<UAureAttributeSet>(AttributeSet))
	{
		//监听血量变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		// 监听HitReact标签
		AbilitySystemComponent->RegisterGameplayTagEvent(FAureGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
				this,
				&AAureEnemy::HitReactTagChanged
			);

		//初始化血量
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
	
}

void AAureEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	Cast<UAureAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	
	//注册监听负面标签变动
	DeBuffRegisterChanged();

	//初始化属性
	InitializeDefaultAttributes();

	//调用ASC广播
	OnASCRegistered.Broadcast(AbilitySystemComponent);

	//打印生命值查看属性
	//UE_LOG(LogTemp, Warning, TEXT("%s 的生命值为 %f"), *this->GetName(), Cast<UAureAttributeSet>(AttributeSet)->GetHealth());
}

void AAureEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
