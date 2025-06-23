// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"

#include "AureGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AureAbilitySystemComponent.h"
#include "AbilitySystem/AureAttributeSet.h"
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

void AAureEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AAureEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AAureEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	

	UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);
	

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

	//初始化属性
	InitializeDefaultAttributes();

	//打印生命值查看属性
	//UE_LOG(LogTemp, Warning, TEXT("%s 的生命值为 %f"), *this->GetName(), Cast<UAureAttributeSet>(AttributeSet)->GetHealth());
}

void AAureEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
